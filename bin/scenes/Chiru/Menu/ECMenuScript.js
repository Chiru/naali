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

    var menudatamodel = MenuComponent.GetMenuDataModel();

    menudatamodel.AddItem("local://mailbox.mesh",["local://mailbox.Material.0.material","local://mailbox.Material.1.material","local://mailbox.Material.2.material","local://mailbox.Material.3.material"]);
    menudatamodel.AddItem("local://chrome.mesh",["local://chrome.Material.0.material","local://chrome.Material.0.material"]);
    menudatamodel.AddItem("local://chatbuble.mesh",["local://chatbuble.Material.0.material","local://chatbuble.Material.1.material"]);
    menudatamodel.AddItem("local://facebook.mesh",["local://facebook.Material.0.material","local://facebook.Material.1.material"]);
    menudatamodel.AddItem("local://notebook.mesh",["local://notebook.Material.0.material"]);
    menudatamodel.AddItem("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    menudatamodel.AddItem("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);
    menudatamodel.AddItem("local://chessboard.mesh",["local://chessboard.Material.0.material","local://chessboard.Material.1.material"]);
    menudatamodel.AddItem("local://notes.mesh",["local://notes.Material.0.material","local://notes.Material.1.material","local://notes.Material.2.material"]);
    menudatamodel.AddItem("local://skype.mesh",["local://skype.Material.0.material"]);
    menudatamodel.AddItem("local://radio.mesh",["local://radio.Material.0.material","local://radio.Material.1.material","local://radio.Material.2.material"]);
    menudatamodel.AddItem("local://file.mesh",["file.Material.0.material","file.Material.1.material","file.Material.2.material","file.Material.3.material","file.Material.4.material","file.Material.5.material","file.Material.6.material","file.Material.7.material"]);
    menudatamodel.AddItem("local://diamonds.mesh",["local://diamonds.Material.0.material","local://diamonds.Material.1.material","local://diamonds.Material.2.material"]);
    menudatamodel.AddItem("local://email.mesh",[""]);
    menudatamodel.AddItem("local://firefox.mesh",["Material.028.material", "Material.029.material"]);
    menudatamodel.AddItem("local://letter.mesh",[""]);    
    menudatamodel.AddItem("local://lock.mesh",["Material.041.material", "Material.042.material", "Material.043.material"]);
    menudatamodel.AddItem("local://map.mesh",["local://map.Material.0.material","local://map.Material.1.material","local://map.Material.2.material"]);
    menudatamodel.AddItem("local://mic.mesh",["local://mic.Material.0.material","local://mic.Material.1.material","local://mic.Material.2.material"]);

    /*
    menudatamodel.AddItem("local://chessboard.mesh",["local://chessboard.Material.0.material","local://chessboard.Material.1.material"]);
    menudatamodel.AddItem("local://notes.mesh",["local://notes.Material.0.material","local://notes.Material.1.material","local://notes.Material.2.material"]);
    menudatamodel.AddItem("local://skype.mesh",["local://skype.Material.0.material"]);
    menudatamodel.AddItem("local://radio.mesh",["local://radio.Material.0.material","local://radio.Material.1.material","local://radio.Material.2.material"]);
    menudatamodel.AddItem("local://file.mesh",["file.Material.0.material","file.Material.1.material","file.Material.2.material","file.Material.3.material","file.Material.4.material","file.Material.5.material","file.Material.6.material","file.Material.7.material"]);
    */


    //facebook icon
    var dataitem = menudatamodel.GetMenuDataItemRaw(3);
    dataitem.AddChildren("local://notebook.mesh",["local://notebook.Material.0.material"]);
    dataitem.AddChildren("local://chatbuble.mesh",["local://chatbuble.Material.0.material","local://chatbuble.Material.1.material"]);
    dataitem.AddChildren("local://radio.mesh",["local://radio.Material.0.material","local://radio.Material.1.material","local://radio.Material.2.material"]);
    dataitem.AddChildren("local://album.mesh",["local://album.Material.0.material","local://album.Material.1.material","local://album.Material.2.material"]);
    dataitem.AddChildren("local://battery.mesh",["local://battery.Material.0.material","local://battery.Material.1.material","local://battery.Material.2.material"]);
    dataitem.AddChildren("local://skype.mesh",["local://skype.Material.0.material"]);

    var layer3 = dataitem.GetChildDataItemRaw(0);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);

    layer3 = dataitem.GetChildDataItemRaw(1);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    layer3.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);

    dataitem = menudatamodel.GetMenuDataItemRaw(4);
    dataitem.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    dataitem.AddChildren("local://radio.mesh",["local://radio.Material.0.material","local://radio.Material.1.material","local://radio.Material.2.material"]);
    dataitem.AddChildren("local://notebook.mesh",["local://notebook.Material.0.material"]);
    dataitem.AddChildren("local://camera.mesh",["local://camera.Material.0.material","local://camera.Material.1.material","local://camera.Material.2.material","local://camera.Material.3.material"]);
    dataitem.AddChildren("local://radio.mesh",["local://radio.Material.0.material","local://radio.Material.1.material","local://radio.Material.2.material"]);
    dataitem.AddChildren("local://notebook.mesh",["local://notebook.Material.0.material"]);

    dataitem = menudatamodel.GetMenuDataItemRaw(5);
    dataitem.AddChildren("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);
    dataitem.AddChildren("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);
    dataitem.AddChildren("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);
    dataitem.AddChildren("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);
    dataitem.AddChildren("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);
    dataitem.AddChildren("local://calculator.mesh",["local://calculator.Material.0.material","local://calculator.Material.1.material","local://calculator.Material.2.material"]);

    //Generate one menu tree from folder with image subfolders.
    generateMenuData(menudatamodel.GetMenuDataItemRaw(11));

    MenuComponent.OnMenuSelectionRaw.connect(MenuItemSelected);
    MenuComponent.PrepareMenuContainer(6.0, menudatamodel);

    //MenuComponent.AddComponentToMenu("local://battery.mesh", ["local://battery.Material.0.material", "local://battery.Material.1.material", "local://battery.Material.2.material"]);

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


function generateMenuData(menudataitem)
{
    var i=0;
    var j=0;

    //Find some way to use relative paths or something?
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

        var dataWidget = new QLabel();
        var fileInfo = dirList[i];
        dataWidget.setText(fileInfo);

        menudataitem.AddChildren(dataWidget);

        //Create images for 3rd layer
        dir.cd(fileInfo);
        var subDirCount = dir.count();
        var subDirList = dir.entryList();

        var images = menudataitem.GetChildDataItemRaw(i-2);
        //items 0 and 1 is '.' and '..', so we are not adding those.
        for(j=2;j<subDirCount;j++)
        {
            var dataWidget = new QLabel();
            dataWidget.pixmap = new QPixmap(dir.filePath(subDirList[j]));
            images.AddChildren(dataWidget);

            //print("i on: " + i + " j on: "+ j + " : " + subDirList[j]);
        }
        dir.cdUp();
        //menuArray[i-2]=menuData;
    }
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

