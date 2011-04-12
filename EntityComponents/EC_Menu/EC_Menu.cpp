/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Template.cpp
 *  @brief  EC_Template is empty template for EC components.
 *          This componen can be used as a template when creating new EC components.
 *  @note   no notes
 */


#include "StableHeaders.h"
#include "EC_Menu.h"
#include "IModule.h"
#include "Entity.h"
#include "LoggingFunctions.h"
#include <Ogre.h>
//#include <QVector3D>
#include <EC_OgreCamera.h>

#include <QStringListModel>
#include <QListView>

#include "SceneManager.h"
#include "UiAPI.h"
#include "SceneAPI.h"

#include <QLabel>

#include "MemoryLeakCheck.h"

DEFINE_POCO_LOGGING_FUNCTIONS("EC_Menu")

EC_Menu::EC_Menu(IModule *module) :
            IComponent(module->GetFramework())
{

    renderTimer_ = new QTimer();
    // Connect signals from IComponent
    connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareMenu()), Qt::UniqueConnection);
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
    QObject::connect(renderTimer_, SIGNAL(timeout()), this, SLOT(Render()));

    renderTimer_->setInterval(40);
    renderTimer_->start();

    SceneInteractWeakPtr sceneInteract = GetFramework()->Scene()->GetSceneIteract();
    if (!sceneInteract.isNull())
    {
        connect(sceneInteract.data(), SIGNAL(EntityClicked(Scene::Entity*, Qt::MouseButton)),
                SLOT(EntityClicked(Scene::Entity*, Qt::MouseButton)));
    }

    //LogInfo("EC_Menu initialized");
}

EC_Menu::~EC_Menu()
{
    /// \todo Write your own EC_Component destructor here
    SAFE_DELETE_LATER(listview_);
}

/*void EC_Menu::EntityClicked(Scene::Entity *entity, Qt::MouseButton button)
{
    //if (!getinteractive() || !GetParentEntity())
    //    return;

    // We are only interested in left clicks on our entity.
    //if (!raycastResult)
      //  return;
    if (button != Qt::LeftButton)
        return;

    if (entity == GetParentEntity())
    {
        // We are only interested in clicks to our target submesh index.
        //if (raycastResult->submesh_ != (unsigned)getrenderSubmeshIndex())
          //  return;

        // Entities have EC_Selected if it is being manipulated.
        // At this situation we don't want to show any ui.
        if (entity->HasComponent("EC_Selected"))
            return;


    }
}*/
void EC_Menu::PrepareMenu()
{
    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    // Get parent and connect to the component removed signal.
    Scene::Entity *parent = GetParentEntity();
    if (parent)
    {
        connect(parent, SIGNAL(ComponentRemoved(IComponent*, AttributeChange::Type)), SLOT(ComponentRemoved(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
    }
    else
    {
        LogError("PrepareComponent: Could not get parent entity pointer!");
        return;
    }

    // Get EC_Mesh component
    EC_Mesh *mesh = GetOrCreateMeshComponent();
    if (!mesh)
    {
        // Wait for EC_Mesh to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }

    else
    {
        mesh->SetName("Testi");
        mesh->setmeshRef("local://rect_plane.mesh");


        //GetOrCreateMeshComponent()->setmeshRef("local://screen.mesh");
    }

    EC_Placeable *placeable = GetOrCreatePlaceableComponent();
    if (!placeable)
    {
        // Wait for EC_Placeable to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }


    // Get or create local EC_3DCanvas component
    EC_3DCanvas *sceneCanvas = GetOrCreateSceneCanvasComponent();
    if (!sceneCanvas)
    {
        LogError("PrepareComponent: Could not get or create EC_3DCanvas component!");
        return;
    }
    else
    {
        /*view_ = new QDeclarativeView;
        view_->setSource(QUrl::fromLocalFile("./data/qmlfiles/testi.qml"));
        view_->setFixedSize(400, 400);*/

        QStringList omaLista;
        omaLista<<"yarr"<<"puuh"<<"mylist1"<<"yarr"<<"puuh"<<"mylist1"<<"yarr"<<"puuh"<<"mylist1"<<"yarr"<<"puuh"<<"mylist1";
        omaLista.append("mylist");
        listview_ = new QListView();

        //QWidget *testiw = new QWidget();
        //QLabel *testl = new QLabel("plaeh", testiw);

        //LogInfo("setModel");
        listview_->setModel(new QStringListModel(omaLista));
        //listview_->model()->
        //listview_->setVisible(true);
        SetEntityPosition();
        //LogInfo("SetWidget");
        GetOrCreateSceneCanvasComponent()->SetWidget(listview_);


    }
}

void EC_Menu::AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType)
{

}

void EC_Menu::Render()
{
    //LogInfo("Trying to render");
    //GetOrCreateSceneCanvasComponent()->Update();


    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    // Get needed components, something is fatally wrong if these are not present but componentPrepared_ is true.
    //EC_Mesh *mesh = GetOrCreateMeshComponent();
    EC_3DCanvas *sceneCanvas = GetOrCreateSceneCanvasComponent();

    // Validate submesh index from EC_Mesh
    /*uint submeshIndex = (uint)getrenderSubmeshIndex();
    if (submeshIndex >= mesh->GetNumSubMeshes())
    {
        /// \note ResetSubmeshIndex() is called with a small delay here, or the ec editor UI wont react to it. Resetting the index back to 0 will call Render() again.
        LogWarning("Render submesh index " + QString::number(submeshIndex).toStdString() + " is illegal, restoring default value.");
        QTimer::singleShot(1, this, SLOT(ResetSubmeshIndex()));
        return;
    }*/

    // Set submesh to EC_3DCanvas if different from current
    //if (!sceneCanvas->GetSubMeshes().contains(submeshIndex))

    //sceneCanvas->SetSubmesh(1);
    sceneCanvas->SetSubmesh(0);

    // Set widget to EC_3DCanvas if different from current
    if (sceneCanvas->GetWidget() != listview_)
        sceneCanvas->SetWidget(listview_);

    sceneCanvas->Update();
}


/*void EC_Menu::ComponentAdded(IComponent *component, AttributeChange::Type change)
{
    LogInfo("In ComponentAdded -slot");
    if(component->TypeName()==EC_Mesh::TypeNameStatic())
    {
        GetOrCreateMeshComponent()->SetName("Testi");
        GetOrCreateMeshComponent()->setmeshRef("local://screen.mesh");
        GetOrCreateMeshComponent()->SetTemporary(true);
    }
    else if (component->TypeName()==EC_Placeable::TypeNameStatic())
    {
        SetEntityPosition();
    }
}*/


void EC_Menu::SetEntityPosition()
{
    Scene::Entity *parent = GetParentEntity();
    Scene::EntityPtr avatarCameraPtr = parent->GetScene()->GetEntityByName("AvatarCamera");
    Scene::EntityPtr freeLookCameraPtr = parent->GetScene()->GetEntityByName("FreeLookCamera");

    distance.z=-20;

    if(avatarCameraPtr)
    {
        Scene::Entity *avatarCamera = avatarCameraPtr.get();
        //EC_Placeable *cameraPlaceable = checked_static_cast<EC_Placeable*>(avatarCamera->GetComponent("EC_Placeable").get());
        cameraPlaceable = checked_static_cast<EC_Placeable*>(avatarCamera->GetComponent("EC_Placeable").get());
        //LogInfo("avatarCamera is active");
    }
    else if(freeLookCameraPtr)
    {
        Scene::Entity *freeLookCamera = freeLookCameraPtr.get();
        cameraPlaceable = checked_static_cast<EC_Placeable*>(freeLookCamera->GetComponent("EC_Placeable").get());
        //LogInfo("freeLookCamera is active");
    }

    if(cameraPlaceable)
    {
        Transform cameraTransform = cameraPlaceable->gettransform();
        Vector3df cameraPosition = cameraTransform.position;

        distance = cameraPlaceable->GetRelativeVector(distance);

        ownEntityPos.x = cameraPosition.x+distance.x;
        ownEntityPos.y = cameraPosition.y+distance.y;
        ownEntityPos.z = cameraPosition.z+distance.z;

        Transform entityTransform;
        entityTransform.position=ownEntityPos;
        entityTransform.rotation=cameraTransform.rotation;

        //hack for turning 3DCanvas face to camera.
        /// \TODO Chance this to work properly.
        //entityTransform.SetRot(entityTransform.rotation.x-90,entityTransform.rotation.y ,entityTransform.rotation.z+180);

        GetOrCreatePlaceableComponent()->settransform(entityTransform);
    }
    else
        LogError("Couldn't get OgreCamera Placeable");

}

EC_Mesh *EC_Menu::GetOrCreateMeshComponent()
{
    if (!GetParentEntity())
        return 0;
    IComponent *iComponent =  GetParentEntity()->GetOrCreateComponent("EC_Mesh", AttributeChange::LocalOnly, false).get();
    EC_Mesh *mesh = dynamic_cast<EC_Mesh*>(iComponent);
    return mesh;
}

EC_3DCanvas *EC_Menu::GetOrCreateSceneCanvasComponent()
{
    if (!GetParentEntity())
        return 0;
    //IComponent *iComponent = parent->GetOrCreateComponentRaw(EC_3DCanvas::TypeNameStatic(), AttributeChange::LocalOnly, false);
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_3DCanvas", AttributeChange::LocalOnly, false).get();
    EC_3DCanvas *canvas = dynamic_cast<EC_3DCanvas*>(iComponent);
    return canvas;
}

EC_Placeable *EC_Menu::GetOrCreatePlaceableComponent()
{
    if (!GetParentEntity())
        return 0;
    //IComponent *iComponent = parent->GetOrCreateComponentRaw(EC_3DCanvas::TypeNameStatic(), AttributeChange::LocalOnly, false);
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    return placeable;
}

void EC_Menu::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{

}
