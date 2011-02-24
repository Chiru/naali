var randomnumber=Math.floor(Math.random()*100)
//console.ExecuteCommand("connect(chiru.cie.fi,2345,Pertti"+randomnumber.toString()+")" )
function AutoConnect()
{
    var isserver = server.IsRunning() || server.IsAboutToStart();
    if(isserver==false){
        client.Login("chiru.cie.fi",2345,"Guest"+randomnumber.toString()," ","udp");
    }
}

frame.DelayedExecute(1).Triggered.connect(this,AutoConnect);
