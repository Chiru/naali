/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Template.cpp
 *  @brief  EC_Template is empty template for EC components.
 *          This componen can be used as a template when creating new EC components.
 *  @note   no notes
 */


#include "StableHeaders.h"
#include "EC_QML.h"
#include "EC_3DCanvas.h"
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

DEFINE_POCO_LOGGING_FUNCTIONS("EC_QML")

EC_QML::EC_QML(IModule *module) :
    IComponent(module->GetFramework()),
    renderSubmeshIndex(this, "Render Submesh", 0),
    interactive(this, "Interactive", false),
    ent_clicked_(false),
    qmlsource(this, "qmlsource", ""),
    qml_ready(false)
{
    renderTimer_ = new QTimer();
    cameraMovementTimer_ = new QTimer();

    // Connect signals from IComponent
    connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareQML()), Qt::UniqueConnection);
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
    connect(this, SIGNAL(AttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(ServerHandleAttributeChange(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
    QObject::connect(renderTimer_, SIGNAL(timeout()), this, SLOT(Render()));
    QObject::connect(cameraMovementTimer_, SIGNAL(timeout()), this, SLOT(SmoothCameraMove()));

    cameraMovementTimer_->setInterval(40);
    camera_ready_ = true;
    renderTimer_->setInterval(40);
    renderTimer_->start();


    renderer_ = module->GetFramework()->GetService<Foundation::RenderServiceInterface>();

    //Create a new input context that menu will use to fetch input.
    input_ = module->GetFramework()->Input()->RegisterInputContext("QMLInput", 100);

    // To be sure that Qt doesn't play tricks on us and miss a mouse release when we're in FPS mode,
    // grab the mouse movement input over Qt.
    input_->SetTakeMouseEventsOverQt(true);

    // Listen on mouse input signals.
    connect(input_.get(), SIGNAL(MouseEventReceived(MouseEvent *)), this, SLOT(HandleMouseInputEvent(MouseEvent *)));
}

EC_QML::~EC_QML()
{
    /// \todo Write your own EC_Component destructor here
    SAFE_DELETE_LATER(qmlview_);
    SAFE_DELETE_LATER(renderTimer_);
    SAFE_DELETE_LATER(cameraMovementTimer_);
}

void EC_QML::PrepareQML()
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
        IComponent *iComponent =  GetParentEntity()->CreateComponent("EC_Mesh", AttributeChange::LocalOnly, false).get();
        mesh_ = dynamic_cast<EC_Mesh*>(iComponent);
        canvas_ = dynamic_cast<EC_3DCanvas*>(iComponent);
    }
    else
    {
        LogError("PrepareComponent: Could not get parent entity pointer!");
        return;
    }

    // Create EC_Mesh component.
    mesh_ = CreateMeshComponents();
    if (!mesh_)
    {
        // Wait for EC_Mesh to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }
    else
    {
        mesh_->setmeshRef("local://rect_plane.mesh");
    }

    EC_Placeable *placeable = GetOrCreatePlaceableComponent();
    if (!placeable)
    {
        // Wait for EC_Placeable to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }

    // Get or create local EC_3DCanvas component
    canvas_ = CreateSceneCanvasComponents();
    if (!canvas_)
    {
        LogError("PrepareComponent: Could not get or create EC_3DCanvas component!");
        return;
    }
    else
    {
        qmlview_ = new QDeclarativeView();
        qmlview_->setSource(QUrl(getqmlsource()));
        QObject::connect(qmlview_, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(QMLStatus(QDeclarativeView::Status)));
        SetEntityPosition();

        canvas_->SetMesh(mesh_);
        canvas_->SetSubmesh(0);
        canvas_->SetWidget(qmlview_);
    }
}

void EC_QML::QMLStatus(QDeclarativeView::Status qmlstatus)
{
    if (framework_->IsHeadless())
        return;
    if (qmlstatus == QDeclarativeView::Ready)
    {
        LogInfo("QDeclarativeView has loaded and created the QML component.");

        if (qmlview_->size().width() > 0 && qmlview_->size().height() > 0)
        {
            qml_ready = true;
        }
        else
        {
            LogInfo("Unable to draw the QML component, because it has no size defined!");
            qml_ready = false;
        }
    }

    else if (qmlstatus == QDeclarativeView::Null)
    {
        LogInfo("QDeclarativeView has no source set.");
        qml_ready = false;
    }
    else if (qmlstatus == QDeclarativeView::Loading)
    {
        LogInfo("QDeclarativeView is loading network data.");
        qml_ready = false;
    }
    else if (qmlstatus == QDeclarativeView::Error)
    {
        LogInfo("One or more errors has occurred.");
        qml_ready = false;
    }
    else
    {
        qml_ready = false;
    }
}


void EC_QML::HandleMouseInputEvent(MouseEvent *mouse)
{
    RaycastResult* result;

    if(mouse->IsLeftButtonDown())
    {
        if (renderer_)
        {
            result = renderer_->Raycast(mouse->X(), mouse->Y());

            if(result->entity_==GetParentEntity())
            {
                ent_clicked_ = true;
            }
            else
            {
                ent_clicked_ = false;
            }
        }
    }
    if(mouse->eventType == MouseEvent::MouseReleased && ent_clicked_)
    {
        ent_clicked_ = false;
        if (renderer_)
        {
            result = renderer_->Raycast(mouse->X(), mouse->Y());
            if(result->entity_==GetParentEntity())
            {
                if(cameraPlaceable)
                {
                    Vector3df dist;
                    EC_Placeable *parentPlaceable = checked_static_cast<EC_Placeable*> (GetParentEntity()->GetComponent("EC_Placeable").get());

                    dist.z=4;
                    dist.y=0;
                    dist = parentPlaceable->GetRelativeVector(dist);

                    Transform cameraTransform = cameraPlaceable->gettransform();
                    Transform viewTransform;

                    Transform entityTransform;
                    entityTransform = parentPlaceable->gettransform();
                    Vector3df viewRotation = entityTransform.rotation;

                    //viewTransform.position = ownEntityPos;
                    viewTransform.position.x = ownEntityPos.x+dist.x;
                    viewTransform.position.y = ownEntityPos.y+dist.y;
                    viewTransform.position.z = ownEntityPos.z+dist.z;
                    viewTransform.rotation = viewRotation;
                    target_transform_ = viewTransform;

                    if (cameraTransform != target_transform_)
                    {
                        camera_ready_ = false;
                        c1 = false;
                        c2 = false;
                        c3 = false;
                        cameraMovementTimer_->start();
                    }
                }
                else
                    LogError("Couldn't get OgreCamera Placeable");
            }
        }
    }
}

void EC_QML::SmoothCameraMove()
{
    if (!camera_ready_)
    {
        EC_Placeable *parentPlaceable = checked_static_cast<EC_Placeable*> (GetParentEntity()->GetComponent("EC_Placeable").get());
        Transform cameraTransform = cameraPlaceable->gettransform();
        Transform parentTransform = parentPlaceable->gettransform();

        float diff_x;
        float diff_y;
        float diff_z;
        float when_to_stop_difference = 0.1;
        int divider = 10; //Affects the speed of camera movement. Higher value -> Slower speed


        diff_x = target_transform_.position.x - cameraTransform.position.x;

        if (diff_x > (-1 * when_to_stop_difference) && diff_x < when_to_stop_difference)
        {
            cameraTransform.position.x = target_transform_.position.x;
            c1 = true;
        }
        else
        {
            cameraTransform.position.x += diff_x / divider;
        }

        diff_y = target_transform_.position.y - cameraTransform.position.y;

         if (diff_y > (-1 * when_to_stop_difference) && diff_y < when_to_stop_difference)
        {
            cameraTransform.position.y = target_transform_.position.y;
            c2 = true;
        }
        else
        {
            cameraTransform.position.y += diff_y / divider;
        }

        diff_z = target_transform_.position.z - cameraTransform.position.z;

         if (diff_z > (-1 * when_to_stop_difference) && diff_z < when_to_stop_difference)
        {
            cameraTransform.position.z = target_transform_.position.z;
            c3 = true;
        }
        else
        {
            cameraTransform.position.z += diff_z / divider;
        }

        cameraPlaceable->settransform(cameraTransform);
        cameraPlaceable->LookAt(parentTransform.position);

        if (c1 && c2 && c3)
        {
            cameraTransform.rotation = parentTransform.rotation;
            cameraPlaceable->settransform(cameraTransform);
            camera_ready_ = true;
        }
    }
    else
        cameraMovementTimer_->stop();
}





void EC_QML::AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType)
{
    if (attribute == &qmlsource)
    {
        qml_ready = false;
        qmlview_->setSource(QUrl(getqmlsource()));
    }
}


void EC_QML::Render()
{
    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    if (qml_ready)
    {
            canvas_->SetSubmesh(0);
            canvas_->Update();
    }
}

void EC_QML::ServerHandleAttributeChange(IAttribute *attribute, AttributeChange::Type changeType)
{
    if (attribute == &qmlsource)
    {
        qml_ready = false;
        qmlview_->setSource(QUrl(getqmlsource()));
    }
}


/*void EC_QML::ComponentAdded(IComponent *component, AttributeChange::Type change)
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


void EC_QML::SetEntityPosition()
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

        GetOrCreatePlaceableComponent()->settransform(entityTransform);
    }
    else
        LogError("Couldn't get OgreCamera Placeable");

}

EC_Mesh* EC_QML::CreateMeshComponents()
{

    if (GetParentEntity())
    {
            IComponent *iComponent =  GetParentEntity()->CreateComponent("EC_Mesh", AttributeChange::LocalOnly, false).get();
            EC_Mesh *mesh = dynamic_cast<EC_Mesh*>(iComponent);
            return mesh;

    }
    else
        LogError("Couldn't get parent entity, returning 0");
    return 0;

}

EC_3DCanvas* EC_QML::CreateSceneCanvasComponents()
{
    IComponent *iComponent = GetParentEntity()->CreateComponent("EC_3DCanvas", AttributeChange::LocalOnly, false).get();
    EC_3DCanvas *canvas = dynamic_cast<EC_3DCanvas*>(iComponent);

    /// \!TODO some error handling would be nice..
    return canvas;
}

EC_Placeable *EC_QML::GetOrCreatePlaceableComponent()
{
    if (!GetParentEntity())
        return 0;
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    return placeable;
}

void EC_QML::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{

}
