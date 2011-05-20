engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

var MenuEntity;
var screenEntity;
var menuArray;

if (!framework.IsHeadless())
{
    screenEntity = scene.GetEntityByNameRaw("Screen");
    if(screenEntity)
    {
        //Connect to signals from 3DCanvasSource. Atm works only in this specific case.
        var canvasSource = screenEntity.GetComponentRaw("EC_3DCanvasSource");
        canvasSource.createMenu.connect(Create3DMenu);
        canvasSource.closeMenu.connect(Close3DMenu);
    }        
}

function Create3DMenu()
{
    MenuEntity = scene.CreateEntityLocalRaw();
    var MenuComponent = MenuEntity.GetOrCreateComponentRaw("EC_MenuContainer");
    scene.EmitEntityCreatedRaw(MenuEntity);
    
    var menuData = generateMenuData();
    MenuComponent.SetMenuData(menuData);    
    MenuComponent.OnMenuSelection.connect(MenuItemSelected);
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
    
    if(screenEntity)
    {
        var canvas = screenEntity.GetComponentRaw("EC_3DCanvas");
        if(canvas)
        {
            var widget = new QWidget();
            widget = menuArray[menuitem].layout().itemAt(submenuitem+1);
            canvas.SetSubmesh(1);            
            canvas.SetWidget(widget);
        }
    }    
}

