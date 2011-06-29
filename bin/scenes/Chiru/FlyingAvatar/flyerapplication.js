// !ref: local://fpsflyer.js

// Avatar application. Will handle switching logic between avatar & freelook camera (clientside), and
// spawning avatars for clients (serverside). Note: this is not a startup script, but is meant to be
// placed in an entity in a scene that wishes to implement avatar functionality.

var avatar_area_size = 10;
var avatar_area_x = 0;
var avatar_area_y = 0;
var avatar_area_z = 20;

var isserver = server.IsRunning();

if (isserver == false) {
    var inputmapper = me.GetOrCreateComponentRaw("EC_InputMapper");
    inputmapper.SetTemporary(true);
    inputmapper.contextPriority = 102;
    inputmapper.RegisterMapping("Ctrl+Tab", "ToggleCamera", 1);

    engine.ImportExtension("qt.core");
    engine.ImportExtension("qt.gui");

    me.Action("ToggleCamera").Triggered.connect(ClientHandleToggleCamera);
} else {
    server.UserAboutToConnect.connect(ServerHandleUserAboutToConnect);
    server.UserConnected.connect(ServerHandleUserConnected);
    server.UserDisconnected.connect(ServerHandleUserDisconnected);
    
    // If there are connected users when this script was added, add av for all of them
    var userIdList = server.GetConnectionIDs();
    if (userIdList.length > 0)
        print("[Avatar Application] Application started. Creating avatars for logged in clients.");

    for (var i=0; i < userIdList.length; i++)
    {
        var userId = userIdList[i];
        var userConnection = server.GetUserConnection(userId);
        if (userConnection != null)
            ServerHandleUserConnected(userId, userConnection);
    }
}

function ClientHandleToggleCamera() {
    // For camera switching to work, must have both the freelookcamera & avatarcamera in the scene
    var freelookcameraentity = scene.GetEntityByNameRaw("FreeLookCamera");
    var avatarcameraentity = scene.GetEntityByNameRaw("FlyingCamera");
    var freecameralistener = freelookcameraentity.GetComponentRaw("EC_SoundListener");
    var avatarent = scene.GetEntityByNameRaw("Flyer" + client.GetConnectionID());
    var avatarlistener = avatarent.GetComponentRaw("EC_SoundListener");
    if ((freelookcameraentity == null) || (avatarcameraentity == null))
        return;
    var freelookcamera = freelookcameraentity.ogrecamera;
    var avatarcamera = avatarcameraentity.ogrecamera;

    if (avatarcamera.IsActive()) {
        freelookcameraentity.placeable.transform = avatarcameraentity.placeable.transform;
        freelookcamera.SetActive();
        freecameralistener.active = true;
        avatarlistener.active = false;
        returnButtonProxy.visible = true;
    } else {
        avatarcamera.SetActive();
        avatarlistener.active = true;
        freecameralistener.active = false;
        returnButtonProxy.visible = false;
    }
    
    // Ask entity to check his camera state
    avatarent.Exec(1, "CheckState");
}

function ServerHandleUserAboutToConnect(connectionID, user) {
    // Uncomment to test access control
    //if (user.GetProperty("password") != "xxx")
    //    user.DenyConnection();
}

function ServerHandleUserConnected(connectionID, user) {
    var flyerEntityName = "Flyer" + connectionID;
    var flyerEntity = scene.CreateEntityRaw(scene.NextFreeId(), ["EC_Script", "EC_Placeable", "EC_AnimationController"]);
    flyerEntity.SetTemporary(true); // We never want to save the flyer entities to disk.
    flyerEntity.SetName(flyerEntityName);
    
    var username = user.GetProperty("username");
    if (user != null) {
	flyerEntity.SetDescription(username);
    }

    var script = flyerEntity.script;
    script.type = "js";
    script.runOnLoad = true;
    var r = script.scriptRef;
    r.ref = "local://fpsflyer.js";
    script.scriptRef = r;

    // Set random starting position for flyer
    var placeable = flyerEntity.placeable;
    var transform = placeable.transform;
    var sticky = me.GetOrCreateComponentRaw("EC_DynamicComponent");
    var prev_transform = sticky.GetAttribute(username);
    if (prev_transform) {
        transform = prev_transform;
    } else {
        transform.pos.x = (Math.random() - 0.5) * avatar_area_size + avatar_area_x;
        transform.pos.y = (Math.random() - 0.5) * avatar_area_size + avatar_area_y;
        transform.pos.z = avatar_area_z;
    }
    placeable.transform = transform;

    scene.EmitEntityCreatedRaw(flyerEntity);
    
    if (user != null) {
        print("[Flyer Application] Created flyer for " + user.GetProperty("username"));
    }
}

function ServerHandleUserDisconnected(connectionID, user) {
    var flyerEntityName = "Flyer" + connectionID;
    var flyerEntity = scene.GetEntityByNameRaw(flyerEntityName);
    var sticky = me.GetOrCreateComponentRaw("EC_DynamicComponent");
    var flyer_transform = flyerEntity.placeable.transform;
    if (flyerEntity != null) {
        var entityID = flyerEntity.id;
        scene.RemoveEntityRaw(entityID);

        if (user != null) {
            var username = user.GetProperty("username");
            print("[Flyer Application] User " + username + " disconnected, destroyed flyer entity.");
            if (!sticky.GetAttribute(username))
                sticky.AddQVariantAttribute(username);
            
            sticky.SetAttribute(username, flyer_transform);
        }
    }
}
