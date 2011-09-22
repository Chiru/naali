// Wetab autoconnect script for demo. Creates 4 connections. Two locals and two remotes.

var doFirst = true;
// Start this connection process when FrameAPI sends first frameUpdate signal
frame.Updated.connect(StartProcess);


function StartProcess()
{
        // Wait 40 seconds before doind any connection attempts because valgrind startup is so long. 
    if (doFirst)
    {
        print("-- Starting connection process!! --");
	doFirst = false;
        frame.DelayedExecute(10).Triggered.connect(this,AutoConnect1);
    }
}
   
function AutoConnect1()
{
    var isserver = server.IsRunning() || server.IsAboutToStart();
    if (!isserver)
    {
        // Make 1st connection to one of the local servers
        client.Login("localhost",2345,"Guest","","tcp");
        // Again wait for valgrind. We dont want to make next connection before 1st connection handshake is even completed.
        frame.DelayedExecute(15).Triggered.connect(this,AutoConnect2);
    }     
}

function AutoConnect2()
{
    // Make 2nd connection to one of the local servers
    client.Login("localhost",2346,"Guest","","tcp");
    // Again we wait for valgrind.
    frame.DelayedExecute(15).Triggered.connect(this,AutoConnect3)
}

function AutoConnect3()
{
    // Make 2nd connection to one of the local servers
    client.Login("chiru.cie.fi",2355,"Guest","","udp");
    // uncomment next line if you want to make another connection
    //frame.DelayedExecute(15).Triggered.connect(this,AutoConnect4)
}
    
function AutoConnect4()
{
    // Make 2nd connection to one of the local servers
    client.Login("chiru.cie.fi",2348,"Guest","","udp");
}

