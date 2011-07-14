engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

//Each menutype must have own js-file to specify proper functionality
var MenuEntity;
var screenEntity;
var menuArray;
var canvasSource;

if (!framework.IsHeadless())
{
    screenEntity = scene.GetEntityByNameRaw("Screen");
    if(screenEntity)
    {
        //Connect to signals from 3DCanvasSource. Atm works only in this specific case.
        canvasSource = screenEntity.GetComponentRaw("EC_3DCanvasSource");
        canvasSource.createMenu.connect(Create3DMenu);
        canvasSource.closeMenu.connect(Close3DMenu);
    }        
}

function Create3DMenu()
{
    MenuEntity = scene.CreateEntityLocalRaw();
    var MenuComponent = MenuEntity.GetOrCreateComponentRaw("EC_MenuContainer");
    scene.EmitEntityCreatedRaw(MenuEntity);
    
    // re-enable next steps if you want to use images as menu data
    /*var menuData = generateMenuData();
    MenuComponent.SetMenuWidgets(menuData);
    */

    MenuComponent.OnMenuSelection.connect(MenuItemSelected);
    MenuComponent.PrepareMenuContainer(4.0);

    //MenuComponent.AddComponentToMenu("local://battery.mesh", ["local://battery.Material.0.material", "local://battery.Material.1.material", "local://battery.Material.2.material"]);
    MenuComponent.AddComponentToMenu("local://mailbox.mesh",["local://mailbox.Material.0.material","local://mailbox.Material.1.material","local://mailbox.Material.2.material","local://mailbox.Material.3.material"]);
    MenuComponent.AddComponentToMenu("local://chrome.mesh",["local://chrome.Material.0.material","local://chrome.Material.0.material"]);
    MenuComponent.AddComponentToMenu("local://chatbuble.mesh",["local://chatbuble.Material.0.material","local://chatbuble.Material.1.material"]);
    MenuComponent.AddComponentToMenu("local://facebook.mesh",["local://facebook.Material.0.material","local://facebook.Material.1.material"]);
    MenuComponent.AddComponentToMenu("local://notebook.mesh",["local://notebook.Material.0.material"]);
    MenuComponent.AddComponentToMenu("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    MenuComponent.AddComponentToMenu("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);
    MenuComponent.AddComponentToMenu("local://chessboard.mesh",["local://chessboard.Material.0.material","local://chessboard.Material.1.material"]);
    MenuComponent.AddComponentToMenu("local://notes.mesh",["local://notes.Material.0.material","local://notes.Material.1.material","local://notes.Material.2.material"]);
    MenuComponent.AddComponentToMenu("local://skype.mesh",["local://skype.Material.0.material"]);
    MenuComponent.AddComponentToMenu("local://radio.mesh",["local://radio.Material.0.material","local://radio.Material.1.material","local://radio.Material.2.material"]);
    MenuComponent.AddComponentToMenu("local://file.mesh",["file.Material.0.material","file.Material.1.material","file.Material.2.material","file.Material.3.material","file.Material.4.material","file.Material.5.material","file.Material.6.material","file.Material.7.material"]);

    MenuComponent.ActivateMenu();
}

function Close3DMenu()
{
    //Delete menu entity
    var EntityId = MenuEntity.id;
    //print("Menu entityId " + EntityId);
    if(EntityId){
        scene.RemoveEntityRaw(EntityId);
    }
    else{
        print("Couldn't get entity ID!");
    }
}


function generateMenuData()
{
    var i=0;
    var j=0;

    var dir = new QDir("/home/juha/Pictures/testData/pictures");
    //print(dir.dirName());
    //print(dir.count());
    var numberOfElements = dir.count();
    menuArray = new Array(numberOfElements);
    //dir.setFilter(QDir::AllDirs);
    //dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    var dirList = dir.entryList();

    //items 0 and 1 is '.' and '..', so we are not adding those.
    // \todo add filter to fix that.
    for(i = 2; i < numberOfElements; ++i)
    {
        var menuData = new QWidget();
        var verticalLayout = new QVBoxLayout();
        //var dataWidget = new QLabel()
        //340x83
        //dataWidget.pixmap = new QPixmap("/home/juha/src/chiru4/bin/data/ui/images/realxtend_logo.png");
        //horizontalLayout.addWidget(verticalLayout,0,0);

        var dataWidget = new QLabel();
        var fileInfo = dirList[i];
        dataWidget.setText(fileInfo);

        //hardcoded fix to see whole text.
        dataWidget.setFixedWidth(65);
        
        //print("i: " + i + " dir: " + fileInfo);
        verticalLayout.addWidget(dataWidget,0,0);
        //verticalLayout.setStretch(i,1);

        dir.cd(fileInfo);
        var subDirCount = dir.count();
        var subDirList = dir.entryList();
        
        //items 0 and 1 is '.' and '..', so we are not adding those.
        for(j=2;j<subDirCount;j++)
        {
            var dataWidget = new QLabel();
            dataWidget.pixmap = new QPixmap(dir.filePath(subDirList[j]));
            verticalLayout.addWidget(dataWidget,0,0);
            //print("i on: " + i + " j on: "+ j + " : " + subDirList[j]);
        }
        //horizontalLayout.addLayout(verticalLayout,0);
        dir.cdUp();
        menuData.setLayout(verticalLayout);
        menuArray[i-2]=menuData;
    }
       
    return menuArray;
}

function MenuItemSelected(menuitem, submenuitem)
{
    print("Menuitem: " + menuitem + " SubmenuItem: " + submenuitem);
    
    if(menuitem==1)
    {
        canvasSource.OpenWebview();
        Close3DMenu();
    }
    else if(menuitem==11)
    {
        //file.mesh

    }


    /*if(screenEntity)
    {
        var canvas = screenEntity.GetComponentRaw("EC_3DCanvas");
        if(canvas)
        {
            var widget = new QWidget();
            widget = menuArray[menuitem].layout().itemAt(submenuitem+1);
            canvas.SetSubmesh(1);            
            canvas.SetWidget(widget);
        }
    } */
}

