// A freelook camera script. Upon run, creates necessary components if they don't exist yet, and hooks to the InputMapper's
// input context to process camera movement (WASD + mouse)

engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

var rotate_sensitivity = 0.3;
var move_sensitivity = 30.0;
var motion_z = 0;
var motion_y = 0;
var motion_x = 0;

if (framework.GetModuleQObj("QMLUIModule"))
{
    qmlmodule = framework.GetModuleQObj("QMLUIModule");
    qmlmodule.Move.connect(QMLMove);
}

if (!me.HasComponent("EC_OgreCamera"))
{
    // Create components & setup default position/lookat for the camera, mimicing RexLogicModule::CreateOpenSimViewerCamera()
    var camera = me.GetOrCreateComponentRaw("EC_OgreCamera");
    var inputmapper = me.GetOrCreateComponentRaw("EC_InputMapper");
    var placeable = me.GetOrCreateComponentRaw("EC_Placeable");
    var soundlistener = me.GetOrCreateComponentRaw("EC_SoundListener");
    soundlistener.active = true;

    camera.AutoSetPlaceable();

    // Co-operate with the AvatarApplication: if AvatarCamera already exists, do not activate the freelookcamera right now
    var avatarcameraentity = scene.GetEntityByNameRaw("AvatarCamera");
    if (!avatarcameraentity)
        camera.SetActive();

    var transform = placeable.transform;
    transform.rot.x = 90;
    placeable.transform = transform;

    // Hook to update tick
    frame.Updated.connect(Update);
    // Register press & release action mappings to the inputmapper, use higher priority than RexMovementInput to be sure
    inputmapper.contextPriority = 101;
    inputmapper.takeMouseEventsOverQt = true;
    inputmapper.modifiersEnabled = false;
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
    inputmapper.RegisterMapping("Up", "Move(forward)", 1);
    inputmapper.RegisterMapping("Down", "Move(back)", 1);
    inputmapper.RegisterMapping("Left", "Move(left)", 1);
    inputmapper.RegisterMapping("Right", "Move(right)", 1);
    inputmapper.RegisterMapping("Up", "Stop(forward)", 3);
    inputmapper.RegisterMapping("Down", "Stop(back)", 3);
    inputmapper.RegisterMapping("Left", "Stop(left)", 3);
    inputmapper.RegisterMapping("Right", "Stop(right)", 3);

    // Connect actions
    me.Action("Move").Triggered.connect(HandleMove);
    me.Action("Stop").Triggered.connect(HandleStop);
    me.Action("MouseLookX").Triggered.connect(HandleMouseLookX);
    me.Action("MouseLookY").Triggered.connect(HandleMouseLookY);

    // Connect gestures
    /*var inputContext = inputmapper.GetInputContext();
    if (inputContext.GestureStarted && inputContext.GestureUpdated)
    {
	inputContext.GestureStarted.connect(GestureStarted);
	inputContext.GestureUpdated.connect(GestureUpdated);
    }*/
}

function IsCameraActive()
{
    var camera = me.GetComponentRaw("EC_OgreCamera");
    return camera.IsActive();
}

function Update(frametime)
{
    if (!IsCameraActive())
    {
        motion_x = 0;
        motion_y = 0;
        motion_z = 0;
        return;
    }

    var placeable = me.placeable;
    if (motion_z != 0)
    {
        var motionvec = new Vector3df();
        motionvec.z = -motion_z * move_sensitivity * frametime;
        placeable.TranslateRelative(motionvec);
    }
    if (motion_x != 0)
    {
        var motionvec = new Vector3df();
        motionvec.x = motion_x * move_sensitivity * frametime;
        placeable.TranslateRelative(motionvec);
    }
    if (motion_y != 0)
    {
        var motionvec = new Vector3df();
        motionvec.y = motion_y * move_sensitivity * frametime;
        placeable.TranslateRelative(motionvec);
    }
}

function HandleMove(param)
{
    if (param == "forward")
        motion_z = 1;
    if (param == "back")
        motion_z = -1;
    if (param == "right")
        motion_x = 1;
    if (param == "left")
        motion_x = -1;
    if (param == "up")
        motion_y = 1;
    if (param == "down")
        motion_y = -1;
}

function HandleStop(param)
{
    if ((param == "forward") && (motion_z == 1))
        motion_z = 0;
    if ((param == "back") && (motion_z == -1))
        motion_z = 0;
    if ((param == "right") && (motion_x == 1))
        motion_x = 0;
    if ((param == "left") && (motion_x == -1))
        motion_x = 0;
    if ((param == "up") && (motion_y == 1))
        motion_y = 0;
    if ((param == "down") && (motion_y == -1))
        motion_y = 0;
}

function HandleMouseLookX(param)
{
    if (!IsCameraActive())
        return;

    var move = parseInt(param);
    var placeable = me.GetComponentRaw("EC_Placeable");
    var newtransform = placeable.transform;
    newtransform.rot.z -= rotate_sensitivity * move;
    placeable.transform = newtransform;
}

function HandleMouseLookY(param)
{
    if (!IsCameraActive())
        return;

    var move = parseInt(param);
    var placeable = me.GetComponentRaw("EC_Placeable");
    var newtransform = placeable.transform;
    newtransform.rot.x -= rotate_sensitivity * move;
    placeable.transform = newtransform;
}

function GestureStarted(gestureEvent)
{
    if (!IsCameraActive())
        return;

    if (gestureEvent.GestureType() == Qt.TapAndHoldGesture)
    {
        if (motion_z == 0)
            HandleMove("forward");
        else
            HandleStop("forward");
        gestureEvent.Accept();
    }
    else if (gestureEvent.GestureType() == Qt.PanGesture)
    {
        var offset = gestureEvent.Gesture().offset.toPoint();
        HandleMouseLookX(offset.x());
        HandleMouseLookY(offset.y());
        gestureEvent.Accept();
    }
}

function GestureUpdated(gestureEvent)
{
    if (!IsCameraActive())
        return;

    if (gestureEvent.GestureType() == Qt.PanGesture)
    {
        var delta = gestureEvent.Gesture().delta.toPoint();
        HandleMouseLookX(delta.x());
        HandleMouseLookY(delta.y());
        gestureEvent.Accept();
    }
}

function QMLMove(direction)
{
    if (direction == "up")
        me.Exec(2, "Move", "forward");
    else if (direction == "stopup")
        me.Exec(2, "Stop", "forward");

    else if (direction == "down")
        me.Exec(2, "Move", "back");
    else if (direction == "stopdown")
        me.Exec(2, "Stop", "back");

    else if (direction == "left")
        me.Exec(2, "Move", "left");
    else if (direction == "stopleft")
        me.Exec(2, "Stop", "left");

    else if (direction == "right")
        me.Exec(2, "Move", "right");
    else if (direction == "stopright")
        me.Exec(2, "Stop", "right");

    else if (direction == "stop")
    {
        me.Exec(2, "StopRotate", "all");
        me.Exec(2, "Stop", "all");
        
    }     
}
