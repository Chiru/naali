// Script for wetab avatar mousemovement. For now this is just a quick hack and has only been tested on desktop pc with mouse.
// author: Jukka Vatjus-Anttila

var SceneAPI = framework.Scene();
var scene;
var avatarent;
var rotate_speed = 150.0;
var mouse_rotate_sensitivity = 0.3;
var move_force = 15.0;
var fly_speed_factor = 0.25;
var damping_force = 3.0;
var walk_anim_speed = 0.5;
var avatar_mass = 10;
var walk = false;

input.takeMouseEventsOverQt = false;
input.TopLevelInputContext().MouseMove.connect(mouseMove);
input.TopLevelInputContext().MouseLeftPressed.connect(mousePress);
input.TopLevelInputContext().MouseLeftReleased.connect(mouseRelease);
SceneAPI.DefaultWorldSceneChanged.connect(start);

engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

function mouseMove(event)
{
    if (walk)
    {
        if (event.IsItemUnderMouse())
            return;
        if (!scene)
            return;
        avatarent = scene.GetEntityByNameRaw("Avatar" + client.GetConnectionID());
        print(client.GetConnectionID());
	    if (!avatarent)
	        return;
        else
        {
	        avatarent.Action("MouseLookX").Triggered.connect(HandleMouseLookX);

	        if (event.relativeX != 0 && event.relativeX < 100 && event.relativeX > -100)
	            avatarent.Exec(2, "MouseLookX", String(mouse_rotate_sensitivity*2 * parseInt(event.relativeX)));
	        if (event.relativeY < -30)
	            avatarent.Exec(2,"Move","forward");
	        else if (event.relativeY > 30)
	            avatarent.Exec(2,"Move","back");
	    }
    }
}

function HandleMouseLookX(param) {
    avatarent = scene.GetEntityByNameRaw("Avatar" + client.GetConnectionID());
    var move = parseInt(param);
    var rotateVec = new Vector3df();
    rotateVec.z = -mouse_rotate_sensitivity * move;
    avatarent.rigidbody.Rotate(rotateVec);
}

function mousePress(event)
{
    if (event.IsItemUnderMouse())
        return;
    if (!scene)
        return;
    avatarent = scene.GetEntityByNameRaw("Avatar" + client.GetConnectionID());
    if (!avatarent)
    {
        print("AvatarEntity not found!");
        return;
    }
	avatarent.Exec(2, "Stop", "forward");
    avatarent.Exec(2, "Stop", "back");
	walk = true;
}

function mouseRelease(event)
{
    if (event.IsItemUnderMouse())
        return;
    if (!scene)
        return;
    avatarent = scene.GetEntityByNameRaw("Avatar" + client.GetConnectionID());
    if (!avatarent)
    {
        print("AvatarEntity not found!");
        return;
    }
	walk = false;
    avatarent.Exec(2, "StopRotate", "all");
}

function start(newScene)
{
    print("Default scene changed: input.js!" + client.GetConnectionID());
    
    scene = newScene;
}
