if (!framework.IsHeadless())
{
    engine.ImportExtension("qt.core");
    engine.ImportExtension("qt.gui");

    var sceneAction = null;
    var assetAction = null;
    
    var mainwin = ui.MainWindow();

    var fileMenu = mainwin.AddMenu("&File");
    
    var importMenu = fileMenu.addMenu(new QIcon("./data/ui/images/folder_closed.png"), "Import Scene");
    importMenu.addAction(new QIcon("./data/ui/images/resource.png"), "From File").triggered.connect(OpenLocalScene);
    importMenu.addAction(new QIcon("./data/ui/images/icon/browser.ico"), "From Web").triggered.connect(OpenWebScene);
    var exportAction = fileMenu.addAction(new QIcon("./data/ui/images/resource.png"), "Export Scene");
    exportAction.triggered.connect(SaveScene);
    fileMenu.addSeparator();
    
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

    var viewMenu = mainwin.AddMenu("&View");
    if (framework.GetModuleQObj("CAVEStereo"))
    {
        var caveMenu = viewMenu.addMenu("&CAVE and Stereo");
        caveMenu.addAction("CAVE").triggered.connect(OpenCaveWindow);
        caveMenu.addAction("Stereoscopy").triggered.connect(OpenStereoscopyWindow);
    }

    // Chiru demonstration and mobility menu:
    var demoMenu = mainwin.AddMenu("&Demo");
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
        //print("Emitted and catched MobilityModule signal with value: " + value);
    }

    if (framework.GetModuleQObj("SceneStructure"))
    {
        assetAction = viewMenu.addAction(new QIcon("./data/ui/images/fileIcons.png"), "Assets");
        assetAction.triggered.connect(OpenAssetsWindow);
        sceneAction = viewMenu.addAction(new QIcon("./data/ui/images/fileList.png"), "Scene");
        sceneAction.triggered.connect(OpenSceneWindow);
    }

    if (framework.GetModuleQObj("Console"))
    {
        viewMenu.addAction("Console").triggered.connect(OpenConsoleWindow);
    }

    //var eceditorAction = viewMenu.addAction("EC Editor");

    if (framework.GetModuleQObj("DebugStats"))
        viewMenu.addAction("Profiler").triggered.connect(OpenProfilerWindow);

    if (framework.GetModuleQObj("Environment"))
    {
        viewMenu.addAction("Terrain Editor").triggered.connect(OpenTerrainEditor);
        viewMenu.addAction("Post-processing").triggered.connect(OpenPostProcessWindow);
    }

    if (framework.GetModuleQObj("PythonScript"))
        viewMenu.addAction("Python Console").triggered.connect(OpenPythonConsole);
        
    var helpMenu = mainwin.AddMenu("&Help");
    helpMenu.addAction(new QIcon("./data/ui/images/icon/browser.ico"), "Wiki").triggered.connect(OpenWikiUrl);
    helpMenu.addAction(new QIcon("./data/ui/images/icon/browser.ico"), "Doxygen").triggered.connect(OpenDoxygenUrl);
    helpMenu.addAction(new QIcon("./data/ui/images/icon/browser.ico"), "Mailing list").triggered.connect(OpenMailingListUrl);
    
    function NewScene() {
        scene.RemoveAllEntities();
    }

    function Reconnect() {
        client.Reconnect();
    }

    function Disconnect() {
        client.Logout();
    }

    function Connected() {
        disconnectAction.setEnabled(true);
        importMenu.setEnabled(true);
        exportAction.setEnabled(true);
        ui.EmitAddAction(sceneAction);
        ui.EmitAddAction(assetAction);
    }

    function Disconnected() {
        disconnectAction.setEnabled(false);
        importMenu.setEnabled(false);
        exportAction.setEnabled(false);
    }

    function Quit() {
        framework.Exit();
    }

    function CheckForUpdates() {
        if (framework.GetModuleQObj("UpdateModule"))
            framework.GetModuleQObj("UpdateModule").RunUpdater("/checknow");
    }

    function OpenMailingListUrl() {
        QDesktopServices.openUrl(new QUrl("http://groups.google.com/group/realxtend/"));
    }
    
    function OpenWikiUrl() {
        QDesktopServices.openUrl(new QUrl("http://wiki.realxtend.org/"));
    }

    function OpenDoxygenUrl() {
        QDesktopServices.openUrl(new QUrl("http://www.realxtend.org/doxygen/"));
    }

    function OpenSceneWindow() {
        framework.GetModuleQObj("SceneStructure").ToggleSceneStructureWindow();
    }

    function OpenAssetsWindow() {
        framework.GetModuleQObj("SceneStructure").ToggleAssetsWindow();
    }

    function OpenProfilerWindow() {
        console.ExecuteCommand("prof");
    }

    function OpenTerrainEditor() {
        framework.GetModuleQObj("Environment").ShowTerrainWeightEditor();
    }

    function OpenPostProcessWindow() {
        framework.GetModuleQObj("Environment").ShowPostProcessWindow();
    }

    function OpenPythonConsole() {
        console.ExecuteCommand("pythonconsole");
    }

    function OpenConsoleWindow() {
        framework.GetModuleQObj("Console").ToggleConsole();
    }

    function OpenStereoscopyWindow() {
        framework.GetModuleQObj("CAVEStereo").ShowStereoscopyWindow();
    }

    function OpenCaveWindow() {
        framework.GetModuleQObj("CAVEStereo").ShowCaveWindow();
    }
    
    function OpenLocalScene() {
        var currentScene = framework.Scene().GetDefaultSceneRaw();
        if (currentScene == null)
            return;
        
        var filename = QFileDialog.getOpenFileName(ui.MainWindow(), "Import Scene", QDir.currentPath() + "/scenes", "Tundra Scene (*.txml *.tbin)");
        if (filename == null || filename == "")
            return;
        if (!QFile.exists(filename))
            return;
            
        var fileninfo = new QFileInfo(filename);
        if (fileninfo.suffix() == "txml")
            currentScene.LoadSceneXML(filename, false, false, 3);
        else if (fileninfo.suffix() == "tbin")
            currentScene.LoadSceneBinary(filename, false, false, 3);
    }
    
    function OpenWebScene() {
        var webRef = QInputDialog.getText(ui.MainWindow(), "Import Scene", "Insert a txml or tbin scene url", QLineEdit.Normal, "http://", Qt.Dialog);
        if (webRef == null || webRef == "")
            return;
        var ext = webRef.substring(webRef.length-4);
        var qUrl = QUrl.fromUserInput(webRef);
        if (!qUrl.isValid())
            return;
        
        if (ext != "txml" && ext != "tbin")
            return;
        var transfer = asset.RequestAsset(qUrl.toString()).get();
        transfer.Loaded.connect(WebSceneLoaded);
    }
    
    function WebSceneLoaded(assetptr) {
        var currentScene = framework.Scene().GetDefaultSceneRaw();
        if (currentScene == null)
            return;
                   
        var asset = assetptr.get();
        var diskSource = asset.DiskSource();
        var fileninfo = new QFileInfo(diskSource);
        if (fileninfo.suffix() == "txml")
            currentScene.LoadSceneXML(diskSource, false, false, 3);
        else if (fileninfo.suffix() == "tbin")
            currentScene.LoadSceneBinary(diskSource, false, false, 3);
    }
    
    function SaveScene() {
        var currentScene = framework.Scene().GetDefaultSceneRaw();
        if (currentScene == null)
            return;
            
        var filename = QFileDialog.getSaveFileName(ui.MainWindow(), "Export Scene", QDir.currentPath() + "/scenes", "Tundra Scene (*.txml *.tbin)");
        if (filename == null || filename == "")
            return;
        var ext = new QFileInfo(filename).suffix();
        if (ext != "txml" && ext != "tbin")
        {
            QMessageBox.information(ui.MainWindow(), "Invalid extension", "Invalid Tundra scene file extension '" + ext + "'");
            return;
        }
           
        if (ext == "txml")
            currentScene.SaveSceneXML(filename);
        else if (ext == "tbin")
            currentScene.SaveSceneBinary(filename);
    }

    // Chiru Mobility changes:

    function ConnectRemote() {
        console.ExecuteCommand("disconnect");
        console.ExecuteCommand("connect(chiru.cie.fi, 2345, 'erkki', '', udp)");
        
    }

    function ConnectLocal() {
        console.ExecuteCommand("disconnect");
        console.ExecuteCommand("connect(localhost, 2345, 'erkki', '', udp)");
    }
    
    function signalNetworkStateChanged() {      
        var options = new Array();
        
        options[0] = "Undetermined";
        options[1] = "Connecting";
        options[2] = "Connected";
        options[3] = "Disconnected";
        options[4] = "Roaming";
        
        var value = QInputDialog.getItem(0, "networkStateChanged(bool)", "value:", options, 0, false, 0);
        
        for(var x=0; x < options.length; x++)
        {
            if(value == options[x]) mobilityModule.networkStateChanged(x);
        }
    }
    
    function signalNetworkModeChanged() {
        var options = new Array();
        
        options[0] = "Unknown";
        options[1] = "Gsm";
        options[2] = "Cdma";
        options[3] = "Wcdma";
        options[4] = "Wlan";
        options[5] = "Ethernet";
        options[6] = "Bluetooth";
        options[7] = "Wimax";
        
        var value = QInputDialog.getItem(0, "networkModeChanged(bool)", "value:", options, 0, false, 0);
        
        for(var x=0; x < options.length; x++)
        {
            if(value == options[x]) mobilityModule.networkModeChanged(x);
        }
    }
    
    function signalNetworkQualityChanged() {
        var value = QInputDialog.getInt(0, "networkQualityChanged(int)", "value(0-100):", 0, 0, 100, 1) 
        if(value) mobilityModule.networkQualityChanged(value);
    }
    
    function signalScreenStateChanged() {
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
    
    function signalUsingBattery() {
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
    
    function signalBatteryLevelChanged() {
        var value = QInputDialog.getInt(0, "batteryLevelChanged(int)", "value(0-100):", 0, 0, 100, 1) 
        if(value) mobilityModule.batteryLevelChanged(value);
    }
    

}
