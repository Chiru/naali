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

    var connectMenu = menu.addMenu("&Connect");
    connectMenu.addAction("Chiru world").triggered.connect(ConnectRemote);
    connectMenu.addAction("Local world").triggered.connect(ConnectLocal);


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
        framework.GetModuleQObj("SceneStructure").ShowSceneStructureWindow();
    }

    function OpenAssetsWindow()
    {
        framework.GetModuleQObj("SceneStructure").ShowAssetsWindow();
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
}
