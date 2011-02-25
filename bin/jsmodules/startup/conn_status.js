var last_update_ms = 0;
function got_update(when) {
    ms_since_update= when;
    now = new Date();
    now_ms = now.getTime();
    last_update_ms = now_ms - when;
}

function check_conn_status() {
    now = new Date();
    now_ms = now.getTime();
    ms_passed = now_ms - last_update_ms;
    if (ms_passed > 1000) {
	print("connection problem? last update " + ms_passed.toFixed(0) + " ms ago");
    }
    frame.DelayedExecute(1).Triggered.connect(this, check_conn_status);
}    

var isserver = server.IsRunning() || server.IsAboutToStart();
if(isserver == false) {
    client.LastHeardUpdate.connect(got_update);
    frame.DelayedExecute(1).Triggered.connect(this, check_conn_status);
}
