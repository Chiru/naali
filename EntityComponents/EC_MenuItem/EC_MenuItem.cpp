/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Menu.cpp
 *  @brief  EC_Menu creates 3D Menu component in to scene.
 *  @note   no notes
 */


#include "StableHeaders.h"
#include "EC_MenuItem.h"
#include "EC_3DCanvas.h"
#include "EC_Mesh.h"
#include "EC_Placeable.h"

#include "IModule.h"
#include "Entity.h"
#include "LoggingFunctions.h"



#include <Ogre.h>
#include <EC_OgreCamera.h>
#include "RenderServiceInterface.h"

#include <QStringListModel>
#include <QListView>
//#include <QMouseEvent>
//#include "MouseEvent.h"
#include "InputAPI.h"

#include "SceneInteract.h"

#include "SceneManager.h"
#include "UiAPI.h"
#include "SceneAPI.h"

#include <QLabel>

#include "MemoryLeakCheck.h"

DEFINE_POCO_LOGGING_FUNCTIONS("EC_MenuItem")

EC_MenuItem::EC_MenuItem(IModule *module) :
    IComponent(module->GetFramework()),
    renderSubmeshIndex(this, "Render Submesh", 0),
    interactive(this, "Interactive", false),
    phi(this, "Phi", 0.0),
    ent_clicked_(false)
{
    // Connect signals from IComponent
    connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareMenuItem()), Qt::UniqueConnection);
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);

    //LogInfo("EC_MenuItem initialized");
}

EC_MenuItem::~EC_MenuItem()
{
    SAFE_DELETE_LATER(listview_);

}

void EC_MenuItem::PrepareMenuItem()
{    
    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    // Get parent and connect to the signals.
    Scene::Entity *parent = GetParentEntity();
    assert(parent);
    if (parent)
    {
        connect(parent, SIGNAL(ComponentRemoved(IComponent*, AttributeChange::Type)), SLOT(ComponentRemoved(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
    }
    else
    {
        LogError("PrepareComponent: Could not get parent entity pointer!");
        return;
    }

    // Create EC_Mesh component.
    EC_Mesh *Mesh_ = GetOrCreateMeshComponent();
    if (!Mesh_)
    {
        // Wait for EC_Mesh to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }
    else
    {
            //MeshList_.at(i)->SetName("meshname"+i);
            Mesh_->setmeshRef("local://rect_plane.mesh");
            //Mesh->SetAdjustPosition(position);
    }

    EC_Placeable *placeable = GetOrCreatePlaceableComponent();
    if (!placeable)
    {
        // Wait for EC_Placeable to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }

    // Get or create local EC_3DCanvas component
    EC_3DCanvas *Canvas_ = GetOrCreateCanvasComponent();
    if (!Canvas_)
    {
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }
    else
    {
        //view_ = new QDeclarativeView;
        //view_->setSource(QUrl::fromLocalFile("./data/qmlfiles/testi.qml"));
        //view_->setFixedSize(400, 400);

        /// \TODO Add dynamic data import mechanishm
        //Some hardcoded testmaterial.

        QStringList TestList;
        TestList<<"list1"<<"list2"<<"list3"<<"list4"<<"list5"<<"list6"<<"list7"<<"list8"<<"list9"<<"list10"<<"list11"<<"list12";

        listview_ = new QListView();

        //LogInfo("setModel");
        listview_->setModel(new QStringListModel(TestList));

        //LogInfo("SetWidget");

        //Canvas_->SetMesh(MeshList_.at(i));
        Canvas_->SetSubmesh(0);
        Canvas_->SetWidget(listview_);
        Canvas_->Update();

    }
}

void EC_MenuItem::Update()
{
    EC_3DCanvas *canvas = GetOrCreateCanvasComponent();
    canvas->SetSubmesh(0);
    canvas->Update();
}

void EC_MenuItem::SetMenuContainerEntity(ComponentPtr MenuContainer)
{

    //setter-function for setting entity position.
    IComponent *iComponent = GetParentEntity()->GetComponent("EC_Placeable").get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    if(placeable && MenuContainer)
        placeable->SetParent(MenuContainer);
    else
        LogInfo("Couldn't get placeable component!");

}

int EC_MenuItem::GetNumberOfSubItems()
{
    /// \todo when adding data, it sets this value to attribute.
    return 7;
}

void EC_MenuItem::SetMenuItemPosition(Vector3df position)
{
    GetOrCreatePlaceableComponent()->SetPosition(position);
}


void EC_MenuItem::AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType)
{
}


EC_Mesh* EC_MenuItem::GetOrCreateMeshComponent()
{
    EC_Mesh *mesh;
    if (GetParentEntity())
    {

        IComponent *iComponent =  GetParentEntity()->GetOrCreateComponent("EC_Mesh", AttributeChange::LocalOnly, false).get();
        mesh = dynamic_cast<EC_Mesh*>(iComponent);
        return mesh;
    }
    else
    {
        LogError("Couldn't get parent entity, returning empty QList");
        return 0;
    }

}

EC_3DCanvas* EC_MenuItem::GetOrCreateCanvasComponent()
{
    if (!GetParentEntity())
        return 0;
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_3DCanvas", AttributeChange::LocalOnly, false).get();
    EC_3DCanvas *canvas = dynamic_cast<EC_3DCanvas*>(iComponent);

    /// \TODO some error handling would be nice..
    return canvas;
}

EC_Placeable *EC_MenuItem::GetOrCreatePlaceableComponent()
{
    if (!GetParentEntity())
        return 0;
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    if(placeable)
        return placeable;
    else
        return 0;
}

void EC_MenuItem::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{

}
