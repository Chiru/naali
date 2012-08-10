framework.Scene().SceneAdded.connect(OnSceneAdded);

var scene = null;
var avatarEntityId = 0;

var commands = [1,3,1,1,1,1,5,10,6,3,1,1,11,12,8,7,4,3,1,9,10,4,5,11,12,8,4,
                1,1,1,4,9,10,8,7,5,4,2,1,11,12,5,7,11,12,6,4,3,9,10];
var index = 0;

function OnSceneAdded(scenename)
{
    // Get pointer to scene through framework
    scene = framework.Scene().GetScene(scenename);
    scene.EntityCreated.connect(OnEntityCreated);
}

function OnEntityCreated(entity, change)
{
    if (!server.IsRunning() && entity.name == "Avatar" + client.connectionId)
    {
        avatarEntityId = entity.id;
        frame.DelayedExecute(1.0).Triggered.connect(ProduceRandomMovement);
//        frame.Updated.connect(ProduceRandomMovement);
    }
}

function ProduceRandomMovement()
{
    var entity = scene.GetEntity(avatarEntityId);

    if (entity == null)
    {
        frame.DelayedExecute(1.0).Triggered.disconnect(ProduceRandomMovement);
        //frame.Updated.disconnect(ProduceRandomMovement);
        return;
    }

    if(index >= commands.length)
    {
        index = 0;
        print("Rewinding");
    }

    var decision = commands[index];
    index += 1;

    print("Moving avatar: " + decision);
    //var decision = Math.floor(Math.random() * 64);

    if (decision == 1)
        entity.Exec(2, "Move", "forward");
    if (decision == 2)
        entity.Exec(2, "Move", "back");
    if (decision == 3)
        entity.Exec(2, "Move", "right");
    if (decision == 4)
        entity.Exec(2, "Move", "left");
    if (decision == 5)
        entity.Exec(2, "Stop", "forward");
    if (decision == 6)
        entity.Exec(2, "Stop", "back");
    if (decision == 7)
        entity.Exec(2, "Stop", "right");
    if (decision == 8)
        entity.Exec(2, "Stop", "left");
    if (decision == 9)
        entity.Exec(1, "Rotate", "right");
    if (decision == 10)
        entity.Exec(1, "StopRotate", "right");
    if (decision == 11)
        entity.Exec(1, "Rotate", "left");
    if (decision == 12)
        entity.Exec(1, "StopRotate", "left");
    frame.DelayedExecute(1.0).Triggered.connect(ProduceRandomMovement);
}
