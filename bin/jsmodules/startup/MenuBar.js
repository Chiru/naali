if (!framework.IsHeadless())
{
    engine.ImportExtension("qt.core");
    engine.ImportExtension("qt.gui");

    var menu = ui.MainWindow().menuBar();
    menu.clear();

    var fileMenu = menu.addMenu("&File");
    if (framework.GetModuleQObj("UpdateModule"))
        fileMenu.addAction(new QIcon("./data/ui/images/icon/update.ico"), "Check Updates").triggered.connect(CheckForUpdates);
    //fileMenu.addAction("New scene").triggered.connect(NewScene);
    // Reconnect menu items for client only
    if (!server.IsAboutToStart())
    {
        var disconnectAction = fileMenu.addAction(new QIcon("./data/ui/images/icon/disconnect.ico"), "Disconnect");
        disconnectAction.triggered.connect(Disconnect);
        client.Connected.connect(Connected);
        client.Disconnected.connect(Disconnected);
        Disconnected();
    }
    fileMenu.addAction(new QIcon("./data/ui/images/icon/system-shutdown.ico"), "Quit").triggered.connect(Quit);

    var viewMenu = menu.addMenu("&View");
    
    var demoMenu = menu.addMenu("&Demo");

    var connectMenu = demoMenu.addMenu("&Connect");
    connectMenu.addAction("Chiru world").triggered.connect(ConnectRemote);
    connectMenu.addAction("Local world").triggered.connect(ConnectLocal);

    if(framework.GetModuleQObj("Mobility"))
    {
        var mobilityModule = framework.GetModuleQObj("Mobility");
        
        mobilityModule.networkStateChanged.connect(mobilitySignalHandler);
        mobilityModule.networkModeChanged.connect(mobilitySignalHandler);
        mobilityModule.networkQualityChanged.connect(mobilitySignalHandler);
        mobilityModule.screenStateChanged.connect(mobilitySignalHandler);
        mobilityModule.usingBattery.connect(mobilitySignalHandler);
        mobilityModule.batteryLevelChanged.connect(mobilitySignalHandler);
        
        var signalMenu = demoMenu.addMenu("&Signal");
        signalMenu.addAction("networkStateChanged(MobilityModule::NetworkState)").triggered.connect(signalNetworkStateChanged);
        signalMenu.addAction("networkModeChanged(MobilityModule::NetworkMode)").triggered.connect(signalNetworkModeChanged);
        signalMenu.addAction("networkQualityChanged(int)").triggered.connect(signalNetworkQualityChanged);
        signalMenu.addAction("screenStateChanged(MobilityModule::ScreenState)").triggered.connect(signalScreenStateChanged);
        signalMenu.addAction("usingBattery(bool)").triggered.connect(signalUsingBattery);
        signalMenu.addAction("batteryLevelChanged(int)").triggered.connect(signalBatteryLevelChanged);
    }
    
    function mobilitySignalHandler(value)
    {
        print("Emitted and catched MobilityModule signal with value: " + value);
    }

    
    if (framework.GetModuleQObj("SceneStructure"))
    {
        viewMenu.addAction("Assets").triggered.connect(OpenAssetsWindow);
        viewMenu.addAction("Scene").triggered.connect(OpenSceneWindow);
    }

    if (framework.GetModuleQObj("Console"))
    {
        viewMenu.addAction("Console").triggered.connect(OpenConsoleWindow);  
    }

    //var eceditorAction = viewMenu.addAction("EC Editor");

    if (framework.GetModuleQObj("DebugStats"))
        viewMenu.addAction("Profiler").triggered.connect(OpenProfilerWindow);

    if (framework.GetModuleQObj("Environment"))
        viewMenu.addAction("Terrain Editor").triggered.connect(OpenTerrainEditor);

    if (framework.GetModuleQObj("PythonScript"))
        viewMenu.addAction("Python Console").triggered.connect(OpenPythonConsole);
        
    var helpMenu = menu.addMenu("&Help");
    helpMenu.addAction(new QIcon("./data/ui/images/icon/browser.ico"), "Wiki").triggered.connect(OpenWikiUrl);
    helpMenu.addAction(new QIcon("./data/ui/images/icon/browser.ico"), "Doxygen").triggered.connect(OpenDoxygenUrl);
    helpMenu.addAction(new QIcon("./data/ui/images/icon/browser.ico"), "Mailing list").triggered.connect(OpenMailingListUrl);

    function NewScene()
    {
        scene.RemoveAllEntities();
    }

    function Reconnect()
    {
        client.Reconnect();
    }

    function Disconnect()
    {
        client.Logout();
    }

    function Connected()
    {
        disconnectAction.setEnabled(true);
    }

    function Disconnected()
    {
        disconnectAction.setEnabled(false);
    }

    function Quit()
    {
        framework.Exit();
    }

    function CheckForUpdates()
    {
        if (framework.GetModuleQObj("UpdateModule"))
            framework.GetModuleQObj("UpdateModule").RunUpdater("/checknow");
    }
       
    function OpenMailingListUrl()
    {
        QDesktopServices.openUrl(new QUrl("http://groups.google.com/group/realxtend/"));
    }
    
    function OpenWikiUrl()
    {
        QDesktopServices.openUrl(new QUrl("http://wiki.realxtend.org/"));
    }
    
    function OpenDoxygenUrl()
    {
        QDesktopServices.openUrl(new QUrl("http://www.realxtend.org/doxygen/"));
    }
    
    function OpenSceneWindow()
    {
        framework.GetModuleQObj("SceneStructure").ToggleSceneStructureWindow();
    }

    function OpenAssetsWindow()
    {
        framework.GetModuleQObj("SceneStructure").ToggleAssetsWindow();
    }

    function OpenProfilerWindow()
    {
        console.ExecuteCommand("prof");
    }

    function OpenTerrainEditor()
    {
        console.ExecuteCommand("TerrainTextureEditor");
    }

    function OpenPythonConsole()
    {
        console.ExecuteCommand("pythonconsole");
    }

    function OpenConsoleWindow()
    {
        framework.GetModuleQObj("Console").ToggleConsole();
    }

    function ConnectRemote()
    {
        console.ExecuteCommand("disconnect");
        console.ExecuteCommand("connect(chiru.cie.fi, 2345, 'erkki', '', udp)");
        
    }

    function ConnectLocal()
    {
        console.ExecuteCommand("disconnect");
        console.ExecuteCommand("connect(localhost, 2345, 'erkki', '', udp)");
    }
    
    function signalNetworkStateChanged()
    {      
        var options = new Array();
        
        options[0] = "Invalid";
        options[1] = "NotAvailable";
        options[2] = "Connecting";
        options[3] = "Connected";
        options[4] = "Closing";
        options[5] = "Disconnected";
        options[6] = "Roaming";
        options[7] = "Unknown";
        
        var value = QInputDialog.getItem(0, "networkStateChanged(bool)", "value:", options, 0, false, 0);
        
        for(var x=0; x < options.length; x++)
        {
            if(value == options[x]) mobilityModule.networkStateChanged(x);
        }
       
    }
    
    function signalNetworkModeChanged()
    {
        var options = new Array();
        
        options[0] = "Unknown";
        options[1] = "Ethernet";
        options[2] = "WLAN";
        options[3] = "2G";
        options[4] = "CDMA2000";
        options[5] = "WCDMA";
        options[6] = "HSPA";
        options[7] = "Bluetooth";
        options[8] = "WiMax";
        
        var value = QInputDialog.getItem(0, "networkModeChanged(bool)", "value:", options, 0, false, 0);
        
        for(var x=0; x < options.length; x++)
        {
            if(value == options[x]) mobilityModule.networkModeChanged(x);
        }
    }
    
    function signalNetworkQualityChanged()
    {
        var value = QInputDialog.getInt(0, "networkQualityChanged(int)", "value(0-100):", 0, 0, 100, 1) 
        if(value) mobilityModule.networkQualityChanged(value);
    }
    
    function signalScreenStateChanged()
    {
        var options = new Array();
        
        options[0] = "Unknown";
        options[1] = "On";
        options[2] = "Dimmed";
        options[3] = "Off";
        options[4] = "Screen saver";
        
        var value = QInputDialog.getItem(0, "screenStateChanged(MobilityModule::ScreenState)", "value:", options, 0, false, 0);
        
        for(var x=0; x < options.length; x++)
        {
            if(value == options[x]) mobilityModule.screenStateChanged(x);
        }
    }
    
    function signalUsingBattery()
    {
        var options = new Array();
        
        options[0] = "false";
        options[1] = "true";
        
        var value = QInputDialog.getItem(0, "usingBattery(bool)", "value:", options, 0, false, 0);
                                          
        switch(value)
        {
        case "false":
            mobilityModule.usingBattery(false);
            break;
        case "true":
            mobilityModule.usingBattery(true);
            break;
        default:
            break;
        }
    }
    
    function signalBatteryLevelChanged()
    {
        var value = QInputDialog.getInt(0, "batteryLevelChanged(int)", "value(0-100):", 0, 0, 100, 1) 
        if(value) mobilityModule.batteryLevelChanged(value);
    }
    
}
