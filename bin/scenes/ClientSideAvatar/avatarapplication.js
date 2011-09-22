// !ref: http://chiru.cie.fi/lvm-connectivity/ClientSideAvatar/avatar_entity.js

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

function CreateAvatarEntity(username, connectionID, avatarEntityName) {
    var avatarEntity = scene.CreateEntityRaw(scene.NextFreeIdPersistent(), ["EC_Script", "EC_Placeable", "EC_AnimationController"]);
    avatarEntity.SetTemporary(true); // We never want to save the avatar entities to disk.
    avatarEntity.SetName(avatarEntityName);

    dc_set(avatarEntity, "connectionID",  connectionID);

    var script = avatarEntity.script;
    script.type = "js";
    script.runOnLoad = true;
    var r = script.scriptRef;
    r.ref = "http://chiru.cie.fi/lvm-connectivity/ClientSideAvatar/avatar_entity.js";
    script.scriptRef = r;

    // Try to restore avatars previous position
    var placeable = avatarEntity.placeable;
    var transform = placeable.transform;
    var prev_transform = dc_get(me, username);

    var avatarEntity = scene.GetEntityByNameRaw(avatarEntityName);

    if (false && prev_transform) {
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

    var username = user.GetProperty("username");
    var avatarEntityName = "Avatar_" + username;
    var avatarEntity = scene.GetEntityByNameRaw(avatarEntityName);

    // avatar entity state might be out of sync 

    if (!avatarEntity) {
	print("no existing av ent found by username" + username + ", creating new");
        avatarEntity = CreateAvatarEntity(username, connectionID, avatarEntityName);
    } else {
	SetAvatarAppearance(avatarEntity, "default");
	dc_set(avatarEntity, "connectionID",  connectionID);
    }

}


function SetAvatarAppearance(ent, mode) {
    var avatar = ent.GetComponentRaw("EC_Avatar");
    if (avatar) {
	print("avatar appearance: EC_avatar exists, removing it");
	print("prev appearanceref was " + avatar.appearanceRef.ref);
	me.RemoveComponentRaw(avatar);
    }
    avatar = me.GetOrCreateComponentRaw("EC_Avatar");
    print("created ec_avatar with appearance " + mode);
    // Set the avatar appearance. This creates the mesh & animationcontroller, once the avatar asset has loaded
    var r = avatar.appearanceRef;
    r.ref = "http://chiru.cie.fi/lvm-connectivity/ClientSideAvatar/" + mode + "_avatar.xml";
    avatar.appearanceRef = r;
    print("new appearanceref is " + avatar.appearanceRef.ref);
    // print("avatar appearance set");
    //avatar.appearanceId = "http://chiru.cie.fi/lvm-connectivity/ClientSideAvatar/away_avatar.xml"

}

function SetFlatitude(av_ent, onoff) {
    // toggle avatar appearance between flat or normal (y scale 0.1 or 1)

    xfrm = av_ent.transform;
    scale = xfrm.scale;
    if (onoff)
	scale.y = .1;
    else
	scale.y = 1;
    xfrm.scale = scale;
    av_ent.transform = xfrm;
    print("flatitude of " + av_ent + " has been set: " + onoff);
}

function ServerHandleUserDisconnected(connectionID, user) {
    var avatarEntityName = "Avatar_" + user;
    var avatarEntity = scene.GetEntityByNameRaw(avatarEntityName);
    if (avatarEntity != null) {
	//SetFlatitude(avatarEntity, true);
	print("clearing connectionid from " + AvatarEntityName);
	dc_set(avatarEntity, "connectionID", "");
	print("connectionID now: '" + dc_get(avatarEntity, "connectionID") + "'");
	SetAvatarAppearance(avatarEntity, "away");
        var av_transform = avatarEntity.placeable.transform;
        var entityID = avatarEntity.Id || avatarEntity.id;
        if (user != null) {
            var username = user.GetProperty("username");
            print("[Avatar Application] User " + username + " disconnected, keeping avatar entity.");
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
