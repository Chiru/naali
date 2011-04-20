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

DEFINE_POCO_LOGGING_FUNCTIONS("EC_Menu")

EC_Menu::EC_Menu(IModule *module) :
    IComponent(module->GetFramework()),
    renderSubmeshIndex(this, "Render Submesh", 0),
    interactive(this, "Interactive", false)
{

    renderTimer_ = new QTimer();
    // Connect signals from IComponent
    connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareMenu()), Qt::UniqueConnection);
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
    QObject::connect(renderTimer_, SIGNAL(timeout()), this, SLOT(Render()));

    renderTimer_->setInterval(40);
    renderTimer_->start();

    renderer_ = module->GetFramework()->GetService<Foundation::RenderServiceInterface>();

    //Create a new input context that menu will use to fetch input.
    input_ = module->GetFramework()->Input()->RegisterInputContext("MenuInput", 100);

    // To be sure that Qt doesn't play tricks on us and miss a mouse release when we're in FPS mode,
    // grab the mouse movement input over Qt.
    input_->SetTakeMouseEventsOverQt(true);

    // Listen on mouse input signals.
    connect(input_.get(), SIGNAL(OnMouseEvent(MouseEvent *)), this, SLOT(HandleMouseInputEvent(MouseEvent *)));

    ent_clicked_ = false;
    save_start_position_ = true;
    //LogInfo("EC_Menu initialized");
}

EC_Menu::~EC_Menu()
{
    /// \todo Write your own EC_Component destructor here
    SAFE_DELETE_LATER(listview_);
}


void EC_Menu::PrepareMenu()
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

    // Create EC_Mesh components.
    /// \TODO When creating menu, this should check number of menuelements and give that as a imput to CreateMeshComponents().
    MeshList_ = CreateMeshComponents(10);
    if (MeshList_.empty())
    {
        // Wait for EC_Mesh to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }
    else
    {
        int i;
        Vector3df position = Vector3df(0.0, 0.0, 0.0);
        for(i=0;i<MeshList_.count();i++)
        {
            position.x=0 + 2 * cos( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
            position.z=4 + 2 * sin( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );

            //MeshList_.at(i)->SetName("MeshName");
            MeshList_.at(i)->setmeshRef("local://rect_plane.mesh");
            MeshList_.at(i)->SetAdjustPosition(position);
        }
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
        omaLista<<"list1"<<"list2"<<"list3"<<"list4"<<"list5"<<"list6"<<"list7"<<"list8"<<"list9"<<"list10"<<"list11"<<"list12";
        omaLista.append("mylist");
        listview_ = new QListView();

        //QWidget *testiw = new QWidget();
        //QLabel *testl = new QLabel("plaeh", testiw);

        //LogInfo("setModel");
        listview_->setModel(new QStringListModel(omaLista));
        SetEntityPosition();
        //LogInfo("SetWidget");
        GetOrCreateSceneCanvasComponent()->SetWidget(listview_);

    }
}

void EC_Menu::HandleMouseInputEvent(MouseEvent *mouse)
{
    QPoint mousePosition;
    mousePosition.setX(mouse->X());
    mousePosition.setY(mouse->Y());

    RaycastResult* result;
    if (renderer_)
    {
        result = renderer_->Raycast(mouse->X(), mouse->Y());
        if(result->entity_==GetParentEntity() && mouse->IsLeftButtonDown() && save_start_position_)
        {
            startPosition_.setX(mouse->X());
            startPosition_.setY(mouse->Y());
            save_start_position_ = false;
            ent_clicked_ = true;
        }
    }

    if(ent_clicked_ && mouse->IsLeftButtonDown() && mouse->eventType == MouseEvent::MouseMove)
    {
        int i;
        //LogInfo("relative movement: " + ToString(mouse->RelativeX()) +"," + ToString(mouse->RelativeY()));
        //LogInfo("HandleMouseInputEvent");
        if((mousePosition-startPosition_).manhattanLength()>15)
        {
            Vector3df position = Vector3df(0.0,0.0,0.0);
            for(i=0;i<MeshList_.count();i++)
            {
                position.x=0 + 2 * cos( i * 2 * Ogre::Math::PI / MeshList_.count() - ((mouse->X() + startPosition_.x())/150));
                position.z=4 + 2 * sin( i * 2 * Ogre::Math::PI / MeshList_.count() - ((mouse->X() + startPosition_.x())/150));

                MeshList_.at(i)->SetAdjustPosition(position);
            }
        }
    }
    if(mouse->eventType == MouseEvent::MouseReleased)
    {
        ent_clicked_ = false;
        save_start_position_ = true;
        //acceleratorVector_.setX(mouse->RelativeX());
        //acceleratorVector_.setY(mouse->RelativeY());
        //kinecticScroller(acceleratorVector_);
    }
    /*void EC_Menu::mouseMoveEvent(QMouseEvent *event)
    {
        if (!(event->buttons() & Qt::LeftButton))
            return;
        if ((event->pos() - dragStartPosition).manhattanLength() < 30)
            return;

        //QDrag *drag = new QDrag(this);
        //QMimeData *mimeData = new QMimeData;

        //mimeData->setData(mimeType, data);
        //drag->setMimeData(mimeData);

        //Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);

    }*/
}
void EC_Menu::kinecticScroller(QPoint a)
{

    LogInfo("kinectic");
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

    distance.z=-10;
    distance.y=-1;

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
        //TODO Chance this to work properly.
        //entityTransform.SetRot(entityTransform.rotation.x-90,entityTransform.rotation.y ,entityTransform.rotation.z+180);

        GetOrCreatePlaceableComponent()->settransform(entityTransform);
    }
    else
        LogError("Couldn't get OgreCamera Placeable");

}

QList<EC_Mesh *> EC_Menu::CreateMeshComponents(int NumberOfMenuObjects)
{
    int i;

    if (GetParentEntity())
    {
        for(i=0; i<NumberOfMenuObjects; i++)
        {
            IComponent *iComponent =  GetParentEntity()->CreateComponent("EC_Mesh", AttributeChange::LocalOnly, false).get();
            EC_Mesh *mesh = dynamic_cast<EC_Mesh*>(iComponent);
            MeshList_.append(mesh);
        }
    }
    else
        LogError("Couldn't get parent entity, returning empty QList");
    return MeshList_;
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
    //IComponent *iComponent = GetParentEntity()->CreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    return placeable;
}

void EC_Menu::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{

}
