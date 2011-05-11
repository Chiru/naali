// !ref: local://avatar_entity.js

// Server-side part of the app, which handles starting the apps on clients 


if (server.IsRunning()) {
    server.UserAboutToConnect.connect(ServerHandleUserAboutToConnect);
    server.UserConnected.connect(ServerHandleUserConnected);
    server.UserDisconnected.connect(ServerHandleUserDisconnected);
} else {
    var inputmapper = me.GetOrCreateComponentRaw("EC_InputMapper");
    inputmapper.SetTemporary(true);
    inputmapper.contextPriority = 102;
}

function ServerHandleUserAboutToConnect(connectionID, user) {
    // Uncomment to test access control
    //if (user.GetProperty("password") != "xxx")
    //    user.DenyConnection();
}

function CreateAvatarEntity(username, avatarEntityName) {
    var avatarEntity = scene.CreateEntityRaw(scene.NextFreeId(), ["EC_Script", "EC_Placeable", "EC_AnimationController"]);
    avatarEntity.SetTemporary(true); // We never want to save the avatar entities to disk.
    avatarEntity.SetName(avatarEntityName);
    var script = avatarEntity.script;
    script.type = "js";
    script.runOnLoad = true;
    var r = script.scriptRef;
    r.ref = "local://avatar_entity.js";
    script.scriptRef = r;

    // Try to restore avatars previous position
    var placeable = avatarEntity.placeable;
    var transform = placeable.transform;
    var sticky = me.GetOrCreateComponentRaw("EC_DynamicComponent");
    var prev_transform = dc_get(me, "username");

    var avatartEntity = scene.GetEntityByNameRaw(avatarEntityName);

    if (prev_transform) {
        transform = prev_transform;
    } else {
        // Set random starting position for avatar
        var avatar_area_size = 10;
        var avatar_area_x = 0;
        var avatar_area_y = 0;
        var avatar_area_z = 20;

        transform.pos.x = (Math.random() - 0.5) * avatar_area_size + avatar_area_x;
        transform.pos.y = (Math.random() - 0.5) * avatar_area_size + avatar_area_y;
        transform.pos.z = avatar_area_z;
    }
    placeable.transform = transform;

    scene.EmitEntityCreatedRaw(avatarEntity);
    return avatarEntity;
}

function log(msg) {
    print("[Avatar app] " + msg);
}

function ServerHandleUserConnected(connectionID, user) {
    if (!user) {
        log("got UserConnected but no user");
        return;
    }
    var avatarEntityName = "Avatar" + connectionID;
    var avatarEntity = scene.GetEntityByNameRaw(avatarEntityName);
    var username = user.GetProperty("username");

    if (!avatarEntity)
        avatarEntity = CreateAvatarEntity(username, avatarEntityName);

    
}

function ServerHandleUserDisconnected(connectionID, user) {
    var avatarEntityName = "Avatar" + connectionID;
    var avatarEntity = scene.GetEntityByNameRaw(avatarEntityName);
    if (avatarEntity != null) {
        var av_transform = avatarEntity.placeable.transform;
        var entityID = avatarEntity.Id;
        if (user != null) {
            var username = user.GetProperty("username");
            print("[Avatar Application] User " + username + " disconnected, destroyed avatar entity.");
            dc_set(me, username, av_transform);
        }
    }
}

function dc_set(ent, key, val) {
    var dc = ent.GetOrCreateComponentRaw("EC_DynamicComponent");
    if (!dc.GetAttribute(key))
        dc.AddQVariantAttribute(key);
    dc.SetAttribute(key, val);
}

function dc_get(ent, key) {
    var dc = me.GetOrCreateComponentRaw("EC_DynamicComponent");
    return dc.GetAttribute(key);
}
