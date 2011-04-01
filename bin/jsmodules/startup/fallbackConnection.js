var address_;
var port_;
var protocol_;
var username_;
var onlocalworld_ = false;

if(framework.GetModuleQObj("Mobility"))
{
    framework.GetModuleQObj("Mobility").networkStateChanged.connect(handleConnection);
}
else
{
    client.FallbackConnection.connect(fallbackConnection);
}

// When mobility is present, use this function for fallback handling.
function handleConnection(param)
{
    if(param == 5)
    {
        address_ = client.GetLoginProperty("address");
        port_ = client.GetLoginProperty("port");
        protocol_ = client.GetLoginProperty("protocol");
        username_ = client.GetLoginProperty("username");
        
        print("Network disconnected, initializing fallback to local world...");
        client.Logout();
        client.Login("localhost",2345,username_,"","udp");
        onlocalworld_ = true;
    }
    else if(param == 3)
    {
        if(onlocalworld_)
        {
            client.Logout();
            client.Login(address_, port_, username_, "", protocol_);
            onlocalworld_ = false;
        }
    }
}

// Use the old fallback function when no mobility is present.
function fallbackConnection()
{
    client.Login("localhost",2345,"user","","udp");
}
