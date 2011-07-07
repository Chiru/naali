engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

var rotate_sensitivity = 0.2;
var scroll_sensitivity = 0.02;

var high_angle = 30.0;
var low_angle = -10.0;

var default_distance = 15.0;
var min_distance = 10.0;
var max_distance = 25.0;

var focused_point;
var zooming = false;
var global_transform;
var drag_enabled = false;

var isserver = server.IsRunning();

if (isserver == false) 
    ClientCreateSphericalCamera();


function ClientCreateSphericalCamera() 
{
    focused_point = scene.GetEntityByNameRaw("Spinpoint1");
    if(focused_point == null)
    {
        print("No Spinpoint found. Sphericalcamera not initialized!");
        return;
    }

    var objectcamera = scene.CreateEntityLocalRaw(scene.NextFreeId());
    var camera = objectcamera.GetOrCreateComponentRaw("EC_OgreCamera");
    var inputmapper = objectcamera.GetOrCreateComponentRaw("EC_InputMapper", 1, false);
    var placeable = objectcamera.GetOrCreateComponentRaw("EC_Placeable");
    
    objectcamera.SetTemporary(true);
    objectcamera.SetName("SphericalCamera");
    
    camera.AutoSetPlaceable();
    camera.SetActive();

    inputmapper.contextPriority = 101;
    inputmapper.takeMouseEventsOverQt = true;
    inputmapper.keyrepeatTrigger = false;
    inputmapper.executionType = 1;    
    
    var inputContext = inputmapper.GetInputContext();
    inputContext.MouseMove.connect(HandleMouseMove);
    inputContext.MouseRightPressed.connect(HandleMousePressed);
    inputContext.MouseRightReleased.connect(HandleMouseReleased);
    inputContext.MouseScroll.connect(mouseScroll);
    
    var focuspos = focused_point.placeable.transform.pos;
    var transform = placeable.transform;
    transform.pos.x = focuspos.x - default_distance;
    transform.pos.y = focuspos.y;
    transform.pos.z = focuspos.z + 5;
    placeable.transform = transform;   
    
    placeable.LookAt(new QVector3D(focuspos.x, focuspos.y, focuspos.z));
    
    print("SphericalCamera initialized");
    scene.EmitEntityCreatedRaw(objectcamera); 
}

function HandleMousePressed()
{
    drag_enabled = true;
}

function HandleMouseReleased()
{
    drag_enabled = false;
}

// \todo This could be split in to a MoveCamera() and HandleMouseMove()
// functions to make adding gesture support easier.
function HandleMouseMove(event)
{       
    if(drag_enabled)
    {
        var width = renderer.GetWindowWidth();
        var height = renderer.GetWindowHeight();

        var x = 4*Math.PI*event.relativeX/width;
        var y = 4*Math.PI*event.relativeY/height;
    
        var sphericalcamera = scene.GetEntityByNameRaw("SphericalCamera");
        var transform = sphericalcamera.placeable.transform;       
        var pos = new Vector3df();
        pos.x = transform.pos.x;
        pos.y = transform.pos.y;
        pos.z = transform.pos.z;

        var pivot = focused_point.placeable.transform.pos;

        var dir = new Vector3df();
        dir.x = pos.x - pivot.x;
        dir.y = pos.y - pivot.y;
        dir.z = pos.z - pivot.z;
        
        // Cap the angles the camera can move
        var angle = Math.asin((pos.z - pivot.z)/getLength(dir)) * (180/Math.PI);
        if(angle >= high_angle && event.relativeY > 0)
            y = 0;
        else if(angle <= low_angle && event.relativeY < 0)
            y = 0;
        
        var quat = QQuaternion.fromAxisAndAngle(sphericalcamera.placeable.LocalYAxis, (-x*180)/Math.PI);
        quat = multiply_quats(quat, QQuaternion.fromAxisAndAngle(sphericalcamera.placeable.LocalXAxis, (-y*180)/Math.PI));
        var dirq = quat.rotatedVector(new QVector3D(dir.x, dir.y, dir.z));
        dir.x = dirq.x();
        dir.y = dirq.y();
        dir.z = dirq.z();

        var new_pos = new Vector3df();
        new_pos.x = pivot.x + dir.x;
        new_pos.y = pivot.y + dir.y;
        new_pos.z = pivot.z + dir.z;

        transform.pos.x = new_pos.x;
        transform.pos.y = new_pos.y;
        transform.pos.z = new_pos.z; 

        sphericalcamera.placeable.transform = transform;
        sphericalcamera.placeable.LookAt(new QVector3D(pivot.x, pivot.y, pivot.z));  
    }
}

function multiply_quats(q, r)
{
    var w = q.scalar()*r.scalar() - q.x()*r.x() - q.y()*r.y() - q.z()*r.z();
    var x = q.scalar()*r.x() + q.x()*r.scalar() + q.y()*r.z() - q.z()*r.y();
    var y = q.scalar()*r.y() + q.y()*r.scalar() + q.z()*r.x() - q.x()*r.z();
    var z = q.scalar()*r.z() + q.z()*r.scalar() + q.x()*r.y() - q.y()*r.x();
    var t = new QQuaternion(w, x, y, z);
    return t;
}

function normalize(vec)
{
    var result = new Vector3df();
    var length = vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;
    if (length == 0)
        return new Vector3df(0, 0, 0);

    length = 1/Math.sqrt(length);
    result.x = vec.x * length;
    result.y = vec.y * length;
    result.z = vec.z * length;

    return result;
}

function getLength(vec)
{
    var result = Math.sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);    
    return result;
}

function mouseScroll(event)
{
    var objectcamera = scene.GetEntityByNameRaw("SphericalCamera");
    var camera = objectcamera.GetComponentRaw("EC_OgreCamera");
    if (camera.IsActive() == false)
        return;
        
    var transform = objectcamera.placeable.transform.pos;
    var focus = focused_point.placeable.transform.pos;

    var dir = new Vector3df();
    dir.x = transform.x - focus.x;
    dir.y = transform.y - focus.y;
    dir.z = transform.z - focus.z;
    
    if(getLength(dir) < min_distance && event.relativeZ > 0)
        return;
    else if(getLength(dir) > max_distance && event.relativeZ < 0)
        return;

    var placeable = objectcamera.GetComponentRaw("EC_Placeable");
    
    var motionvec = new Vector3df();
    motionvec.z = -event.relativeZ * scroll_sensitivity;
    placeable.TranslateRelative(motionvec);
}
