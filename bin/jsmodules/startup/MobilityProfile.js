var FPSLOW = 5.0;
var FPSDEFAULT = 20.0;
var FPSNOLIMIT = 0.0;

var NETWORKQUALITYPOOR = 40;
var NETWORKQUALITYGOOD = 60;

var onlocalworld = false;
var address;
var port;
var username;
var protocol;

// For testing purposes, we need to store the 'fake' values emitted by mobility, since
// querying these directly from mobilitymodule returns the real world values.
var usingbattery;
var batterylevel;
var batterycritical;
var networkstate;
var networkmode;
var networkquality;
var screenstate;



if (!framework.IsHeadless())
{
    if(framework.GetModuleQObj("Mobility"))
    {
        mobility = framework.GetModuleQObj("Mobility");
        
        usingbattery = mobility.GetUsingBattery();
        batterylevel = mobility.GetBatteryLevel();
        batterycritical = mobility.GetBatteryCritical();
        networkstate = mobility.GetNetworkState();
        networkmode = mobility.GetNetworkMode();
        networkquality = mobility.GetNetworkQuality();
        screenstate = mobility.GetScreenState();

        mobility.batteryLevelChanged.connect(HandleBatteryLevelChanged);
        mobility.batteryLevelCritical.connect(HandleBatteryLevelCritical);
        mobility.usingBattery.connect(HandleUsingBattery);
        mobility.networkStateChanged.connect(HandleNetworkStateChanged);
        mobility.networkModeChanged.connect(HandleNetworkModeChanged);
        mobility.networkQualityChanged.connect(HandleNetworkQualityChanged);
        mobility.screenStateChanged.connect(HandleScreenStateChanged);
    }
    if(framework.GetModuleQObj("QMLUIModule"))
    {
        var qmlui = framework.GetModuleQObj("QMLUIModule")
    }
    if(framework.GetModuleQObj("TundraLogic"))
    {
        var tundralogic = framework.GetModuleQObj("TundraLogic")
    }
}

function HandleBatteryLevelChanged(param)
{
    this.batterylevel = param;
    //qmlui.BatteryLevelChanged(param);
}

function HandleBatteryLevelCritical()
{
    this.batterycritical = param;
    this.DetermineFPSLimit();
    this.DetermineBitpacking();
    this.DeterminePacketBuffering();
}

function HandleUsingBattery(param)
{
    this.usingbattery = param;
    this.DetermineFPSLimit();
    this.DetermineBitpacking();
    this.DeterminePacketBuffering();
    //qmlui.usingBattery(param);
}

function HandleNetworkStateChanged(param)
{
    this.networkstate = param;
    this.DetermineConnectivityStatus();
    //qmlui.NetworkStateChanged(param);
}

function HandleNetworkModeChanged(param)
{
    this.networkmode = param,
    this.DetermineConnectivityStatus();
}

function HandleNetworkQualityChanged(param)
{
    this.networkquality = param;
    this.DetermineBitpacking();
    //qmlui.NetworkQualityChanged(param);
}

function HandleScreenStateChanged(param)
{
    this.screenstate = param;
    this.DetermineRenderingState();
    this.DetermineFPSLimit();
}

function DetermineFPSLimit()
{
    // Battery critical OR Display dimmed
    if(this.batterycritical || this.screenstate == 2)
    {
        framework.renderer.SetTargetFPSLimit(FPSLOW);
    }
    // Display On AND Using battery AND NOT Battery critical
    else if(this.screenstate == 1 && this.usingbattery && !this.batterycritical)
    {
        framework.renderer.SetTargetFPSLimit(FPSDEFAULT);
    }
    // NOT Using battery
    else if(!this.usingbattery)
    {
        framework.renderer.SetTargetFPSLimit(FPSNOLIMIT);
    }
    // Shouldn't get here
    else 
    {
        print("MobilityLogic: Error in determining FPS limit! Setting limit to default..");
        framework.renderer.SetTargetFpsLimit(FPSDEFAULT);
    }
}

function DetermineRenderingState()
{
    // Display ON
    if(this.screenstate == 1 || this.screenstate == 2)
    {
        framework.renderer.SetRenderingEnabled(true);
    }
    // Display Off OR Display saver
    else if(this.screenstate == 3 || this.screenstate == 4)
    {
        framework.renderer.SetRenderingEnabled(false);
    }
    // Shouldn't get here (unless screenstate == undetermined)
    else
    {
        print("MobilityLogic: Error in determining rendering state! Defaulting to ON..");
        framework.renderer.SetRenderingEnabled(true);
    }
}

function DetermineBitpacking()
{
    // Network quality poor OR Battery critical
    if(this.networkquality < NETWORKQUALITYPOOR || this.batterycritical) // This logic seems bit off?
    {
        // Start bitpacking and compression
    }
    // Network quality good OR NOT Battery critical
    else if(this.networkquality > NETWORKQUALITYGOOD || !this.batterycritical) // --"--
    {
        // Stop bitpacking and compression
    }
    else
    {
    }
}

function DeterminePacketBuffering()
{
    if(this.batterycritical)
    {
        // Start packet buffering
    }
    else if(!this.usingbattery)
    {
        // Stop packet buffering
    }
    else
    {
    }
}

function DetermineConnectivityStatus()
{
        //qmlui.NetworkStateChanged(mobility.GetNetworkState());

        if(this.networkstate == 2 && onlocalworld)
        {
            client.Logout();
            client.Login(address, port, username, "", protocol);
            onlocalworld = false;
        }
        else if(this.networkstate == 3 && !onlocalworld)
        {
            address = client.GetLoginProperty("address");
            port = client.GetLoginProperty("port");
            protocol = client.GetLoginProperty("protocol");
            username = client.GetLoginProperty("username");

            client.Logout();
            client.Login("localhost",2345,username,"","udp");
            onlocalworld = true;
        }
}
