// !ref: http://chiru.cie.fi/lvm-assets/Osprey.mesh
// !ref: http://chiru.cie.fi/lvm-assets/Osprey.skeleton
// !ref: http://chiru.cie.fi/lvm-assets/leathers.002.material
// !ref: http://chiru.cie.fi/lvm-assets/body.001.material

engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

// Customization variables to suit your taste.
var roll_sensitivity = 0.4;         // Sensitivity of the 'roll' effect
var rotate_sensitivity = 0.2;       // Sensitivity of pitch and yaw
var stabilization_speed = 0.02;     // Speed of the stabilization, higher equals faster [0..1]
var stabilization_waitout = 400;    // Time to wait after release of the mouse before stabilizing (msecs)
var invert_mouse_y = true;          // Swap mouse y-axis
var max_roll = 30.0;                // Amount of roll when yawing
var move_force = 30.0;              // Force applied when moving (moving speed)
var damping_force = 1.0;            // Damping force

// Global variables, don't touch.
var dest_orientation = new QQuaternion();
var start_orientation = new QQuaternion();
var timer = new QTimer();
var current_roll = 0;
var slerp_t = 0;
var stabilize = false;
var start_roll = 0;
var motion_x = 0;
var motion_y = 0;
var motion_z = 0;

// Helper constants
var RADTODEG = 180.0/Math.PI;
var DEGTORAD = Math.PI/180.0;
var camerarotationoffset = multiply_quats(QQuaternion.fromAxisAndAngle(0,0,1,-90), QQuaternion.fromAxisAndAngle(1,0,0,90));

var isserver = server.IsRunning();
if (isserver) {
    ServerInitialize();
} else {
    ClientInitialize();
}

function ServerInitialize() 
{
    var mesh = me.GetOrCreateComponentRaw("EC_Mesh");
    var meshRef = mesh.meshRef;
    meshRef.ref = "http://chiru.cie.fi/lvm-assets/Osprey.mesh";
    mesh.meshRef = meshRef;
    var skeletonRef = mesh.skeletonRef;
    skeletonRef.ref = "http://chiru.cie.fi/lvm-assets/Osprey.skeleton";
    mesh.skeletonRef = skeletonRef;
    var materials = mesh.meshMaterial;  
    materials = ["http://chiru.cie.fi/lvm-assets/leathers.002.material", "http://chiru.cie.fi/lvm-assets/body.001.material"];
    mesh.meshMaterial = materials;
    
    var trans = mesh.nodeTransformation;
    trans.rot.z = 90;
    mesh.nodeTransformation = trans;
    
    var rigidbody = me.GetOrCreateComponentRaw("EC_RigidBody");
    rigidbody.RespectMyAuthority(false);
}

function ClientInitialize()
{
    if (me.GetName() == "Flyer" + client.GetConnectionID()) {
        var inputmapper = me.GetOrCreateComponentRaw("EC_InputMapper");
        var placeable = me.GetOrCreateComponentRaw("EC_Placeable");
        
        var rigidbody = me.GetOrCreateComponentRaw("EC_RigidBody");
        rigidbody.RespectMyAuthority(true);
        
        var sizeVec = new Vector3df();
        sizeVec.z = 2.4;
        sizeVec.x = 0.5;
        sizeVec.y = 0.5;
        
        rigidbody.mass = 10;
        rigidbody.shapeType = 3; // Capsule
        rigidbody.size = sizeVec;
        rigidbody.gravityEnabled = false
        
        rigidbody.GetPhysicsWorld().Updated.connect(ClientUpdatePhysics);
        
        frame.Updated.connect(ClientUpdate);
        me.Action("Move").Triggered.connect(ClientHandleMove);
        me.Action("Stop").Triggered.connect(ClientHandleStop);
        me.Action("MouseLookX").Triggered.connect(ClientHandleMouseLookX);
        me.Action("MouseLookY").Triggered.connect(ClientHandleMouseLookY);
        
        rigidbody.PhysicsCollision.connect(ClientHandleCollision);
        
        // Hook to update tick
        frame.Updated.connect(ClientUpdate);
        // Register press & release action mappings to the inputmapper, use higher priority than RexMovementInput to be sure
        inputmapper.contextPriority = 101;
        inputmapper.SetNetworkSyncEnabled(false);
        inputmapper.takeMouseEventsOverQt = true;
        inputmapper.modifiersEnabled = false;
        inputmapper.executionType = 1;
        inputmapper.RegisterMapping("W", "Move(forward)", 1);
        inputmapper.RegisterMapping("S", "Move(back)", 1);
        inputmapper.RegisterMapping("A", "Move(left)", 1);
        inputmapper.RegisterMapping("D", "Move(right)", 1);
        inputmapper.RegisterMapping("Space", "Move(up)", 1);
        inputmapper.RegisterMapping("C", "Move(down)", 1);
        inputmapper.RegisterMapping("W", "Stop(forward)", 3);
        inputmapper.RegisterMapping("S", "Stop(back)", 3);
        inputmapper.RegisterMapping("A", "Stop(left)", 3);
        inputmapper.RegisterMapping("D", "Stop(right)", 3);
        inputmapper.RegisterMapping("Space", "Stop(up)", 3);
        inputmapper.RegisterMapping("C", "Stop(down)", 3);
        
        //var inputContext = inputmapper.GetInputContext();
        //inputContext.MouseMove.connect(ClientHandleMouseMove);
        
        timer.timeout.connect(StartStabilization);
        ClientCreateCamera();
    }
}

function ClientCreateCamera()
{
    if (scene.GetEntityByNameRaw("AvatarCamera") != null) {
        return;
    }

    var cameraentity = scene.CreateEntityRaw(scene.NextFreeIdLocal());
    cameraentity.SetName("FlyingCamera");
    cameraentity.SetTemporary(true);

    var camera = cameraentity.GetOrCreateComponentRaw("EC_OgreCamera");
    var placeable = cameraentity.GetOrCreateComponentRaw("EC_Placeable");
    
    camera.AutoSetPlaceable();
    camera.SetActive();
    
    var transform = placeable.transform;
    transform.rot.x = 90;
    transform.pos.x = me.placeable.transform.pos.x;
    transform.pos.y = me.placeable.transform.pos.y - 10;
    transform.pos.z = me.placeable.transform.pos.z;
    
    placeable.transform = transform;
}

function ClientUpdateCamera()
{
    var cameraentity = scene.GetEntityByNameRaw("FlyingCamera");
    if (cameraentity == null)
        return;
    var cameraplaceable = cameraentity.placeable;
    var avatarplaceable = me.placeable;

    var cameratransform = cameraplaceable.transform;
    var avatartransform = avatarplaceable.transform;
    
    var avatarorientation = avatarplaceable.GetQOrientation();
    var neworientation = multiply_quats(avatarorientation, camerarotationoffset);
    var newrotation = to_euler(neworientation);
    
    cameratransform.rot.x = newrotation.x() * RADTODEG;
    cameratransform.rot.y = newrotation.y() * RADTODEG;
    cameratransform.rot.z = newrotation.z() * RADTODEG;
    cameratransform.pos.x = avatartransform.pos.x;
    cameratransform.pos.y = avatartransform.pos.y;
    cameratransform.pos.z = avatartransform.pos.z;
    
    cameraplaceable.transform = cameratransform;
}

function StartStabilization()
{
    var placeable = me.placeable;
    start_orientation = placeable.GetQOrientation();
    var current_rotation = placeable.transform.rot;
    
    var dest_rotation = new Vector3df();
    dest_rotation.x = 0;
    dest_rotation.y = 0;
    dest_rotation.z = current_rotation.z;
    
    dest_orientation = rotation_to_orientation(dest_rotation);
    
    start_roll = current_roll;
    slerp_t = 0;
    stabilize = true;
    timer.stop();
}

function ClientStabilizeAvatar()
{ 
    if(!stabilize)
        return;
        
    // We're done here
    if(slerp_t >= 1) {
        stabilize = false;
        current_roll = 0;
        slerp_t = 0;
        return;
    }
    
    var placeable = me.placeable;
    var rigidbody = me.rigidbody;
 
    var next_orientation = QQuaternion.slerp(start_orientation, dest_orientation, slerp_t);
    var qrotation = to_euler(next_orientation);
    slerp_t += stabilization_speed;
    
    var nextrotation = new Vector3df();
    nextrotation.x = qrotation.x() * RADTODEG;
    nextrotation.y = qrotation.y() * RADTODEG;
    nextrotation.z = qrotation.z() * RADTODEG;
    
    current_roll -= ( start_roll / (1/stabilization_speed) );
        
    rigidbody.SetRotation(nextrotation);
}

function ClientHandleMouseLookX(param)
{        
    timer.stop();
    timer.start(stabilization_waitout);
    stabilize = false;
    
    var move = parseInt(param);
    var degrees = param * roll_sensitivity;
    var placeable = me.GetComponentRaw("EC_Placeable")
    var transform = placeable.transform;
    var rigidbody = me.rigidbody;
    
    var newrotation = new Vector3df();
    newrotation.z = transform.rot.z - (rotate_sensitivity * move);
    
    if(!((current_roll >= max_roll && degrees > 0) || (current_roll <= -max_roll && degrees < 0))) {
        current_roll += degrees;
        
        var oldorientation = placeable.GetQOrientation();
        var roll = QQuaternion.fromAxisAndAngle(1,0,0,degrees);
        var quat = multiply_quats(oldorientation,roll);

        var qrotation = to_euler(quat);
        
        newrotation.x = qrotation.x() * RADTODEG;
        newrotation.y = qrotation.y() * RADTODEG;
    }
    else {
        newrotation.x = transform.rot.x;
        newrotation.y = transform.rot.y;
    }
    
    rigidbody.SetRotation(newrotation);
}


function ClientHandleMouseLookY(param)
{
    timer.stop();
    timer.start(stabilization_waitout);
    stabilize = false;

    if(invert_mouse_y)
        param *= -1;
    
    var degrees = param * rotate_sensitivity; // Fix this to something proper
    var placeable = me.GetComponentRaw("EC_Placeable");
    var rigidbody = me.rigidbody;
    
    var oldorientation = placeable.GetQOrientation();
    var roll = QQuaternion.fromAxisAndAngle(0,1,0,degrees);
    var quat = multiply_quats(oldorientation,roll);   
    var qrotation = to_euler(quat);
    
    var newrotation = new Vector3df();
    newrotation.x = qrotation.x() * RADTODEG;
    newrotation.y = qrotation.y() * RADTODEG;
    newrotation.z = qrotation.z() * RADTODEG;
    
    rigidbody.SetRotation(newrotation);
}

function ClientHandleMove(param)
{
    if (param == "forward") {
        motion_x = 1;
    }
    if (param == "back") {
        motion_x = -1;
    }
    if (param == "right") {
        motion_y = 1;
    }
    if (param == "left") {
        motion_y = -1;
    }
    if (param == "up") {
        motion_z = 1;
    }
    if (param == "down") {
        motion_z = -1;
    }
}

function ClientHandleStop(param)
{
    if ((param == "forward") && (motion_x == 1)) {
        motion_x = 0;
    }
    if ((param == "back") && (motion_x == -1)) {
        motion_x = 0;
    }
    if ((param == "right") && (motion_y == 1)) {
        motion_y = 0;
    }
    if ((param == "left") && (motion_y == -1)) {
        motion_y = 0;
    }
    if ((param == "up") && (motion_z == 1)) {
        motion_z = 0;
    }
    if ((param == "down") && (motion_z == -1)) {
        motion_z = 0;
    }
}

function ClientUpdate(frametime)
{
    /*if (!IsCameraActive())
    {
        motion_x = 0;
        motion_y = 0;
        motion_z = 0;
        return;
    }*/
    
    // Dirty hack, fix focus before shipping.
    if(!IsCameraActive()) {
        var cameraentity = scene.GetEntityByNameRaw("FlyingCamera");
        cameraentity.ogrecamera.SetActive();
    }
    
    ClientStabilizeAvatar()
    ClientUpdateCamera();
}

function ClientUpdatePhysics(frametime)
{
    var placeable = me.placeable;
    var rigidbody = me.rigidbody;

    // Apply motion force
    // If diagonal motion, normalize
    if ((motion_x != 0) || (motion_y != 0) || (motion_z != 0)) {
        var mag = 1.0 / Math.sqrt(motion_x * motion_x + motion_y * motion_y + motion_z * motion_z);
        var impulseVec = new Vector3df();
        impulseVec.x = mag * move_force * motion_x;
        impulseVec.y = -mag * move_force * motion_y;
        impulseVec.z = mag * move_force * motion_z;
        impulseVec = placeable.GetRelativeVector(impulseVec);
        rigidbody.ApplyImpulse(impulseVec);
    }

    // Apply damping. Only do this if the body is active, because otherwise applying forces
    // to a resting object wakes it up
    if (rigidbody.IsActive()) {
        var dampingVec = rigidbody.GetLinearVelocity();
        dampingVec.x = -damping_force * dampingVec.x;
        dampingVec.y = -damping_force * dampingVec.y;
        dampingVec.z = -damping_force * dampingVec.z;
    
        rigidbody.ApplyImpulse(dampingVec);
    }
}

function ClientHandleCollision(ent, pos, normal, distance, impulse, newCollision)
{
    rigidbody = me.rigidbody;
    rigidbody.SetAngularVelocity(new Vector3df(0,0,0));
    
    // Start stabilization timer and apply some additional impulse.
    // Just a quick test to get some kind of a crash effect,
    // should be implemented properly.
    if(newCollision)
    {
        timer.stop();
        timer.start(stabilization_waitout);
        stabilize = false;
        
        normal.x *= -impulse * 2;
        normal.y *= -impulse * 2;
        normal.z *= -impulse * 2;
        rigidbody.ApplyImpulse(normal);
    }
}

function IsCameraActive()
{
    var cameraentity = scene.GetEntityByNameRaw("FlyingCamera");
    if (cameraentity == null)
        return false;
    var camera = cameraentity.ogrecamera;
    return camera.IsActive();
}

function ServerUpdate(frametime)
{
}

function rotation_to_orientation(rot) // Vector3df
{
    rot.x *= DEGTORAD;
    rot.y *= DEGTORAD;
    rot.z *= DEGTORAD;
    
    var q0 = Math.cos(rot.x/2)*Math.cos(rot.y/2)*Math.cos(rot.z/2) + Math.sin(rot.x/2)*Math.sin(rot.y/2)*Math.sin(rot.z/2);
    var q1 = Math.sin(rot.x/2)*Math.cos(rot.y/2)*Math.cos(rot.z/2) + Math.cos(rot.x/2)*Math.sin(rot.y/2)*Math.sin(rot.z/2);
    var q2 = Math.cos(rot.x/2)*Math.sin(rot.y/2)*Math.cos(rot.z/2) + Math.sin(rot.x/2)*Math.cos(rot.y/2)*Math.sin(rot.z/2);
    var q3 = Math.cos(rot.x/2)*Math.cos(rot.y/2)*Math.sin(rot.z/2) + Math.sin(rot.x/2)*Math.sin(rot.y/2)*Math.cos(rot.z/2);
    
    return new QQuaternion(q0, q1, q2, q3);
}

function to_euler(quat) // QQuaternion
{
    var x = Math.atan2(2.0*(quat.scalar()*quat.x()+quat.y()*quat.z()), 1.0-2.0*(quat.x()*quat.x()+quat.y()*quat.y()));
    var y = Math.asin(2.0*(quat.scalar()*quat.y() - quat.z()*quat.x()));
    var z = Math.atan2(2.0*(quat.scalar()*quat.z()+quat.x()*quat.y()),1.0-2.0*(quat.y()*quat.y()+quat.z()*quat.z()));
    return new QVector3D(x, y, z);
}

function multiply_quats(q, r) // QQuaternion, QQuaternion
{
    var w = q.scalar()*r.scalar() - q.x()*r.x() - q.y()*r.y() - q.z()*r.z();
    var x = q.scalar()*r.x() + q.x()*r.scalar() + q.y()*r.z() - q.z()*r.y();
    var y = q.scalar()*r.y() + q.y()*r.scalar() + q.z()*r.x() - q.x()*r.z();
    var z = q.scalar()*r.z() + q.z()*r.scalar() + q.x()*r.y() - q.y()*r.x();
    var t = new QQuaternion(w, x, y, z);
    return t;
}



