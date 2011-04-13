var address_;
var port_;
var protocol_;
var username_;
var onlocalworld_ = false;

if (!framework.IsHeadless())
{
    if(framework.GetModuleQObj("Mobility"))
    {
        framework.GetModuleQObj("Mobility").networkStateChanged.connect(handleConnectionState);
        framework.GetModuleQObj("Mobility").networkModeChanged.connect(handleConnectionMode);
    }
    else
    {
        client.FallbackConnection.connect(fallbackConnection);
    }
}

// When mobility is present, use this function for fallback handling.
function handleConnectionState(param)
{
    if(param == 3)
    {
        if(!onlocalworld_)
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
    }
    else if(param == 2)
    {
        if(onlocalworld_)
        {
            client.Logout();
            client.Login(address_, port_, username_, "", protocol_);
            onlocalworld_ = false;
        }
    }
}

function handleConnectionMode()
{
    print("handlemode triggered!");
    if(!onlocalworld_)
    {
        address_ = client.GetLoginProperty("address");
        port_ = client.GetLoginProperty("port");
        protocol_ = client.GetLoginProperty("protocol");
        username_ = client.GetLoginProperty("username");
        
        client.Logout();
        client.Login(address_, port_, username_, "", protocol_);
    }
}

// Use the old fallback function when no mobility is present.
function fallbackConnection()
{
    client.Login("localhost",2345,"user","","udp");
}
