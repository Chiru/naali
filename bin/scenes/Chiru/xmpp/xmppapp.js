engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

incoming_msgs = [];
nr_msgs = 0;

// function text_bubble(text, x, y, z, lifetime) {
//     var eid = scene.NextFreeId();
//     var ent = scene.CreateEntityRaw(eid, ["EC_Placeable", "EC_Mesh", "EC_3DCanvas"]);
//     var uppercube = scene.GetEntityByNameRaw("uppercube");
//     ent.mesh = uppercube.mesh;
//     print("cloned mesh");
//     ent.mesh.meshMaterial = uppercube.mesh.meshMaterial;
//     ent.mesh.meshRef = uppercube.mesh.meshRef;
//     print("creating 3dcanvas");
//     var canvas = ent.GetOrCreateComponentRaw("EC_3DCanvas");
//     var label = new QLabel(text);
//     label.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed);
//     label.setGeometry(new QRect(0, 0, 500, 500));
//     // var layout = new QGridLayout();
//     layout.setGeometry(new QRect(0, 0, 500, 500));
//     layout.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed);
//     label.resize(500, 500);
//     after(.1, function() { canvas.Update(); });
//     layout.resize(600, 600);
//     layout.AddWidget(label, 0, 0);
//     canvas.SetSubmesh(0);
//     canvas.SetWidget(layout);
//     canvas.Update();
//     canvas.Start();
//     var trans = ent.placeable.transform;
//     pos = trans.pos;
//     //trans.pos = pos;
//     //print("got pos " + pos);
//     print("pos now " + pos.x+ " " + pos.y + " " + pos.z);
//     pos.x = x; pos.y = y; pos.z = z;
//     print("pos now " + pos.x+ " " + pos.y + " " + pos.z);
//     frame.DelayedExecute(lifetime).Triggered.connect(function () {
// 	    scene.RemoveEntityRaw(eid);
// 	});
// }

// frame.DelayedExecute(1).Triggered.connect(this, function () {
// 	text_bubble("asdfasdfasdfas", 0, 10, 10, 100);
//     });

function move_relative(ent, x, y, z) {
    //print("moving " + ent);
    pos = ent.placeable.transform.pos;
    tr = ent.placeable.transform;
    pos = tr.pos;
    pos.z += z;
    pos.x += x;
    pos.y += y;
    tr.pos = pos;
    ent.placeable.transform = tr;

}

function move_absolute(ent, x, y, z) {
    pos = ent.placeable.transform.pos;
    tr = ent.placeable.transform;
    pos = tr.pos;
    pos.z = z;
    pos.x = x;
    pos.y = y;
    tr.pos = pos;
    print("using pos" + ent.placeable.transform.pos.x);
    ent.placeable.transform = tr;
    print("moved " + ent.id + " to " + ent.placeable.transform.pos.x);
	   
}

function rotate_absolute(ent, x, y, z) {
    rot = ent.placeable.transform.rot;
    tr = ent.placeable.transform;
    rot = tr.rot;
    rot.z = z;
    rot.x = x;
    rot.y = y;
    tr.rot = rot;
    print("using rot" + ent.placeable.transform.rot.x);
    ent.placeable.transform = tr;
    print("rotated " + ent.id + " to " + ent.placeable.transform.rot.x);
	   
}

function set_mass(ent, val) {
    print("old gravity " + ent.rigidbody.mass);
    ent.rigidbody.mass = val;
    print("updated gravity " + ent.rigidbody.mass);
}

function after(delay, what) {
    frame.DelayedExecute(delay).Triggered.connect(what);
}

function drop(bub) {
    print("dropping bubble id=" + bub.id);
    //move_relative(bub, 0, 0, -10);
    //function nudgeup() { move_relative(bub, 0, 0, 10); after(4, nudgeup); }
    after(2, function () { set_mass(bub, .1); });
    //after(4, nudgeup);
    var canvas = bub.GetOrCreateComponentRaw("EC_3DCanvas");
    var msg = incoming_msgs.pop();
    if (!msg) {
	print("no msg in incoming_msgs");
	msg = " ";
    }
    if (!canvas.GetWidget()) {
	var ql = new QLabel();
	ql.resize(300, 80);
	ql.setStyleSheet("font: \"Arial\" 20pt");
	var mesh = bub.GetComponentRaw("EC_Mesh");
	if (mesh)
	    var sms = mesh.GetNumSubMeshes();
	else
	    print("no mesh!");
	print("submeshes: " + sms);
	canvas.SetSubmesh(1);
	canvas.SetWidget(ql);
    }
    ql.text = msg;
    after(.1, function() { canvas.Update(); });
	
    canvas.Update();
    canvas.Start();
}

curr_bubble = 0;

after(.5, function () {
	print("finding ids of chatbubbles");
	// no api to get list of same named objects, this assumes they have
	// low ids
	var hanging_b = [];
	var dropped_b = [];
	var startpos = startrot = null;

	for (var i = 0; i < 100; i++) {
	    var ent = scene.GetEntityRaw(i);
	    if (!ent) {
		//print("no ent with id=" + i);
		continue;
	    }
	    if (ent.name != "chatbubble") {
		print("wrong name: " + ent.name);
		continue;
	    }
	    print("found chatbubble id=" + i);
	    hanging_b.push(ent);
	}
	chat_msgs = ["hello", "kekkis", "multimedia", "cyber"];

	startpos = hanging_b[0].placeable.transform.pos;
	startrot = hanging_b[0].placeable.transform.rot;

	function drop_nth (i) {
	    console.ExecuteCommand("stopphysics");
	    print("stopped physics, moving " + hanging_b[i].id);
	    startpos.y -= .2;
	    print(" from its pos " + hanging_b[i].placeable.transform.pos.x);
	    move_absolute(hanging_b[i], startpos.x, startpos.y, startpos.z);
	    rotate_absolute(hanging_b[i], startrot.x, startrot.y, startrot.z);
	    print("dropping " + i + " from pos " + hanging_b[i].placeable.transform.position);
	    bub = hanging_b[i]; move_relative(bub, 0, -10, 0);
	    console.ExecuteCommand("startphysics");
	    drop(hanging_b[i]);
	    hanging_b[i].GetComponentRaw("EC_3DCanvas").Update();
	}
	
	function flip() {
	    while (dropped_b.length) {
		var bub = dropped_b.pop();
		move_absolute(bub, startpos.x, startpos.y, startpos.z);
		hanging_b.push(bub);
	    }
	}

	// for (i = 0; i < hanging_b.length; i++) {
	//     var bub = hanging_b[i];
	//     print("length " + hanging_b.length + " vs i " + i);
	//     print("got bub " + bub.id);
	//     function paa() {
	// 	print("hello " + bub.id + " " + i);
	// 	drop(bub);
	// 	dropped_b.push(bub);
	//     }	    
	//     after(i, paa);
	// }
	// after(i+1, flip);

	// for (i = 0; i < hanging_b.length; i++) {
	//     function paa() {
	// 	var bub = hanging_b.pop();
	// 	print("hello " /*+ bub*/ + " " + i);
	// 	drop(bub);
	// 	dropped_b.push(bub);
	//     }	    
	//     after(i, paa);
	// }
	// after(i+1, flip);

	// function schedule_drops() {

	//     var i=0;
	//     after(i++*2, function() { drop_nth(0) });
	//     after(i++*2, function() { drop_nth(1) });
	//     after(i++*2, function() { drop_nth(2) });
	//     after(i++*2, function() { drop_nth(3) });
	//     after(i++*2, schedule_drops);
	// }

	// schedule_drops();

	function queue_msg(text) {
	    nr_msgs += 1;
	    incoming_msgs.push(text);
	    drop_nth(nr_msgs % 4);
	}

	if (0) {
	    after(1, function() { queue_msg("testing chat messages"); } );
	    after(4, function() { queue_msg("roly poly little chat messages dropping from the sky"); } );
	    after(6, function() { queue_msg("testing chat messages"); } );
	    after(7, function() { queue_msg("roly poly little chat messages dropping from the sky"); } );
	    after(8, function() { queue_msg("testing chat messages"); } );
	    after(9, function() { queue_msg("roly poly little chat messages dropping from the sky"); } );
	}

	function show_url(url) {
	    var screen = scene.GetEntityByNameRaw("Screen");
	    if (!screen) {
		print("no screen found");
		return;
	    }
	    var cs = screen.GetComponentRaw("EC_3DCanvasSource");
	    if (!cs) {
		print("no canvassource found on screen");
		return;
	    }

	    if (url == cs.source)
		return;

	    cs.source = url;
	    cs.refreshRate = 1;
	    print("CanvasSource URL update done (to " + url + ")");
	}
	var chat_from = null;
	var jid2url = {};
	function handle_chat_received(sender, msg) {
	    if (msg.length <= 0)
		return;
	    print("got msg " + msg);
	    queue_msg(msg + "\n(from: " + sender + ")");
	    chat_from = sender;
	    if (msg.indexOf("https:") == 0 || msg.indexOf("http:") == 0) {
		jid2url[sender] = msg;
	    } 
	    if (jid2url[sender]) {
		print("url from jid2url: " + jid2url[sender]);
		show_url(jid2url[sender]);
	    }
	}

	xmppmodule = framework.GetModuleQObj("XMPP");
	var xmppclient;
	if (!(xmppclient =xmppmodule.getClient("chiru.cie.fi"))) {
	    xmppclient = xmpp.newClient("chiru.cie.fi",
					"erno@chiru.cie.fi", "Edienoo9");
	}
	chatext = xmppclient.getExtension("Chat") || xmppclient.addExtension("Chat");
	if (!chatext) {
	    print("Failed to add chat extension");
	}

	chatext.messageReceived.connect(handle_chat_received);

	function handle_connected() {
	    print("xmpp connected");
	    chatext.sendMessage("erno@dukgo.com", "troloo");
	    print("test message sent");
	}
	xmppclient.connected.connect(handle_connected);

	function handle_vcard_changed(jid) {
	    var roster = xmppclient.getRoster();
	    print ("roster update. dumping roster:");
	    for (i in roster) {
		var user = xmppclient.getUser(roster[i]);
		print("user: " + user.getJid());
		print("url: " + user.getUrl());
		print("-");
		if (chat_from == user.getJid()) {
		    print("showing vcard url for " + chat_from);
		    show_url(user.getUrl());
		}
		jid2url[user.getJid()] = user.getUrl();
	    }
	}
	xmppclient.vCardChanged.connect(handle_vcard_changed);
    });

// var chatwidget = ui.LoadFromFile("local://PrivateChatWidget.ui", false);

var webscreen = scene.GetEntityByNameRaw("uppercube");
