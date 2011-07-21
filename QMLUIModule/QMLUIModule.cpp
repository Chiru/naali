/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file QMLUIModule.cpp
 *  @brief QMLUIModule is used for showing a 2D overlay QML UI
 */

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "QMLUIModule.h"
#include "UiServiceInterface.h"
#include "EventManager.h"
#include "ModuleManager.h"
#include "SceneAPI.h"
#include "UiAPI.h"
#include "AssetAPI.h"
#include "EC_Script.h"
#include "AssetCache.h"
#include "InputAPI.h"
#include "UiMainWindow.h"
#include"UiGraphicsView.h"
#include "Entity.h"
#include "RenderServiceInterface.h"
#include "MouseEvent.h"
#include "EC_OgreCamera.h"
#include "math.h"
//#include "qmath.h"

std::string QMLUIModule::type_name_static_ = "QMLUIModule";

QMLUIModule::QMLUIModule() :
    IModule(type_name_static_)

{
    view_created_ = false;
    camera_focused_on_entity = false;
    camera_moving = false;
    entity_movable = false;
    camera_movement_timer_ = new QTimer();
    camera_movement_timer_->setInterval(40);
    QObject::connect(camera_movement_timer_, SIGNAL(timeout()), this, SLOT(SmoothCameraMove()));
    //For QML-debugging
    //QByteArray data = "1";
    //qputenv("QML_IMPORT_TRACE", data);
}

QMLUIModule::~QMLUIModule()
{
}

void QMLUIModule::PreInitialize()
{
}

void QMLUIModule::Initialize()
{
    connect(GetFramework()->Scene(), SIGNAL(SceneAdded(const QString&)), this, SLOT(SceneAdded(const QString&)));
}

void QMLUIModule::PostInitialize()
{
    if (framework_->IsHeadless())
        return;
    //CreateQDeclarativeView();
    //Create a new input context that QMLUIModule will use to fetch input.
    input_ = framework_->Input()->RegisterInputContext("QMLUIInput", 100);
    input_->SetTakeMouseEventsOverQt(true);

    // Listen on mouse input signals.
    connect(input_.get(), SIGNAL(MouseEventReceived(MouseEvent *)), this, SLOT(HandleMouseInputEvent(MouseEvent *)));
    //connect(input_.get(), SIGNAL(GestureEventReceived(GestureEvent*)), this, SLOT(HandleGestureInputEvent(GestureEvent *)));



}

void QMLUIModule::Uninitialize()
{
    //SAFE_DELETE(declarativeview_);
}

void QMLUIModule::Update(f64 frametime)
{
    RESETPROFILER;
}
/*
// virtual
bool QMLUIModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data)
{    
    return false;
}*/

//void QMLUIModule::HandleGestureInputEvent(GestureEvent *event)
//{
//    return;
//}

Scene::Entity* QMLUIModule::GetActiveFreeLookCamera() const
{
    Scene::ScenePtr scene = GetFramework()->Scene()->GetDefaultScene();
    if (!scene)
    {
        LogError("Failed to find active camera, default world scene wasn't setted.");
        return 0;
    }

    foreach(Scene::EntityPtr cam, scene->GetEntitiesWithComponent(EC_OgreCamera::TypeNameStatic()))
        if (cam->GetComponent<EC_OgreCamera>()->IsActive())
        {
            Scene::Entity *cam_ent = cam.get();
            if (cam_ent->GetName() == "FreeLookCamera")
                return cam.get();
            else
            {
                LogError("FreeLookCamera not found or it is not active!");
                return 0;
            }

        }

    LogError("No active camera were found.");
    return 0;
}

Scene::Entity* QMLUIModule::GetActiveCamera() const
{
    Scene::ScenePtr scene = GetFramework()->Scene()->GetDefaultScene();
    if (!scene)
    {
        LogError("Failed to find active camera, default world scene wasn't setted.");
        return 0;
    }

    foreach(Scene::EntityPtr cam, scene->GetEntitiesWithComponent(EC_OgreCamera::TypeNameStatic()))
    if (cam->GetComponent<EC_OgreCamera>()->IsActive())
    {
        return cam.get();
    }

    LogError("No active camera were found.");
    return 0;
}

void QMLUIModule::StartDrag(MouseEvent *mouse)
{
    last_pos_x = mouse->X();
    last_pos_y = mouse->Y();
    float width = renderer_->GetWindowWidth();
    float height = renderer_->GetWindowHeight();
    float normalized_width = 1 / width;
    float normalized_height = 1 / height;
    last_abs_x = normalized_width * mouse->X();
    last_abs_y = normalized_height * mouse->Y();
    entity_movable = true;
}

void QMLUIModule::HandleMouseInputEvent(MouseEvent *mouse)
{

    if(mouse->eventType == MouseEvent::MousePressed && mouse->button == MouseEvent::LeftButton)
    {
        RaycastResult* result;
        if (renderer_)
        {
            result = renderer_->Raycast(mouse->X(), mouse->Y());

            if(result->entity_ && result->entity_->HasComponent("EC_Placeable"))
            {
                entity_to_rotate_ = result->entity_;
                entity_to_drag_ = result->entity_;
                StartDrag(mouse);
                IComponent *iComponent =  result->entity_->GetComponent("EC_Placeable").get();
                EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
                moving_placeable_ = placeable;
            }
        }
    }

    if(mouse->eventType == MouseEvent::MouseMove && entity_movable)
    {
        DragEntity(entity_to_drag_, mouse);

        RotateEntityHorizontally(entity_to_rotate_, mouse->X() - last_pos_x);
        last_pos_x = mouse->X();
    }

    if(mouse->eventType == MouseEvent::MouseReleased && mouse->button == MouseEvent::LeftButton)
    {
        entity_movable = false;
    }

    if (mouse->eventType == MouseEvent::MouseScroll && entity_movable)
    {
        //LogInfo(ToString(mouse->RelativeZ()));
    }

    if (mouse->eventType == MouseEvent::MouseDoubleClicked && mouse->button == MouseEvent::LeftButton)
    {
        if (!camera_focused_on_entity)
        {
            RaycastResult* result;
            if (renderer_)
            {
                result = renderer_->Raycast(mouse->X(), mouse->Y());

                if(result->entity_ && result->entity_->HasComponent("EC_Placeable"))
                {
                    FocusCameraOnEntity(result->entity_);
                }
            }
        }
        else {

        }
    }
}

void QMLUIModule::DragEntity(Scene::Entity *entity, MouseEvent *mouse)
{
    if (!entity->HasComponent("EC_Terrain")) //We don't want to move terrain
    {
        IComponent *iComponent = entity->GetComponent("EC_Placeable").get();
        EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
        Transform entityPos = placeable->gettransform();
        float width = renderer_->GetWindowWidth();
        float height = renderer_->GetWindowHeight();
        float normalized_width = 1 / width;
        float normalized_height = 1/ height;
        float mouse_abs_x = normalized_width * mouse->X();
        float mouse_abs_y = normalized_height * mouse->Y();

        float movedx = mouse_abs_x - last_abs_x;
        float movedy = mouse_abs_y - last_abs_y;

        Scene::Entity *cam = GetActiveCamera();

        iComponent =  cam->GetComponent("EC_OgreCamera").get();
        EC_OgreCamera *camera = dynamic_cast<EC_OgreCamera*>(iComponent);
        iComponent = cam->GetComponent("EC_Placeable").get();
        camera_ = dynamic_cast<EC_Placeable*>(iComponent);

        float fov = camera->GetVerticalFov();
        Vector3df campos = camera_->gettransform().position;
        Vector3df entpos = moving_placeable_->gettransform().position;
        float length = campos.getDistanceFrom(entpos);

        float worldwidth = (tan(fov/2) * length) * 2;
        float worldheight = (height*worldwidth) / width;

        float movefactor = width / height;

        float amountx = (worldwidth * movedx) * movefactor;
        float amounty = (worldheight * movedy) * movefactor;

        last_abs_x = mouse_abs_x;
        last_abs_y = mouse_abs_y;

        entityPos.position += camera_->GetOrientation() * Vector3df(amountx, -1 * amounty, 0);
        placeable->settransform(entityPos);
    }
    else
    {
        return;
    }

}

void QMLUIModule::RotateEntityHorizontally(Scene::Entity *entity, float rotation)
{
    if (!entity->HasComponent("EC_Terrain")) //We don't want to move terrain
    {
        IComponent *iComponent =  entity->GetComponent("EC_Placeable").get();
        EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
        Transform ent_trans = placeable->gettransform();
        ent_trans.rotation += Vector3df(0, 0, rotation);
        placeable->settransform(ent_trans);
    }
    else
    {
        return;
    }
}

void QMLUIModule::FocusCameraOnEntity(Scene::Entity *entity)
{
    if (entity->HasComponent("EC_Placeable"))
    {
        if (GetActiveFreeLookCamera() != 0)
        {
            IComponent *iComponent = GetActiveFreeLookCamera()->GetComponent("EC_Placeable").get();
            camera_ = dynamic_cast<EC_Placeable*>(iComponent);
            iComponent = entity->GetComponent("EC_Placeable").get();
            entity_to_focus_on_ = dynamic_cast<EC_Placeable*>(iComponent);
            camera_movement_timer_->start();
        }
    }
}

void QMLUIModule::SmoothCameraMove()
{

    Transform camera_transform = camera_->gettransform();
    Transform entity_transform = entity_to_focus_on_->gettransform();
    bool camx = false;
    bool camy = false;
    bool camz = false;
    float diff_x;
    float diff_y;
    float diff_z;
    float when_to_stop_difference_x = 5;
    float when_to_stop_difference_y = 5;
    float when_to_stop_difference_z = 5;
    int divider = 50; //Affects the speed of camera movement. Higher value -> Slower speed

    diff_x = entity_transform.position.x - camera_transform.position.x;

    if (diff_x > (-1 * when_to_stop_difference_x) && diff_x < when_to_stop_difference_x)
    {
        camx = true;
    }
    else
    {
        camera_transform.position.x += diff_x / divider;
    }

    diff_y = entity_transform.position.y - camera_transform.position.y;

    if (diff_y > (-1 * when_to_stop_difference_y) && diff_y < when_to_stop_difference_y)
    {
        camy = true;
    }
    else
    {
        camera_transform.position.y += diff_y / divider;
    }

    diff_z = entity_transform.position.z - camera_transform.position.z;

    if (diff_z > (-1 * when_to_stop_difference_z) && diff_z < when_to_stop_difference_z)
    {
        camz = true;
    }
    else
    {
        camera_transform.position.z += diff_z / divider;
    }

    camera_->SetPosition(camera_transform.position);
    camera_->LookAt(entity_transform.position);
    LogInfo("Diffx: " + ToString(diff_x) + " Diffy: " + ToString(diff_y) + " Diffz: " + ToString(diff_z));

    if (camx && camy && camz)
    {
        camera_->LookAt(entity_transform.position);
        //camera_focused_on_entity = true;
        camera_movement_timer_->stop();
    }
}




void QMLUIModule::CreateQDeclarativeView()
{
    //Prepare QDeclarativeView
    if (framework_->IsHeadless() || view_created_)
        return;

    view_created_ = true;

    declarativeview_ = new QDeclarativeView(framework_->Ui()->MainWindow());
    //declarativeview_->setFixedSize(framework_->Ui()->GraphicsView()->viewport()->size());
    QObject::connect(declarativeview_, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(QMLStatus(QDeclarativeView::Status)));

    declarativeview_->move(0,0);
    declarativeview_->setStyleSheet("QDeclarativeView {background-color: transparent;}");
    declarativeview_->setWindowState(Qt::WindowFullScreen);
    declarativeview_->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    declarativeview_->setFocusPolicy(Qt::NoFocus);


    //qmluiproxy_ = new UiProxyWidget(declarativeview_, Qt::Widget);

    //framework_->Ui()->AddProxyWidgetToScene(qmluiproxy_);

    //declarativeview_->setSource(QUrl("/home/olli/src/git/qml-gestures-examples/flickablelist/main.qml"));
    //declarativeview_->setSource(QUrl("../QMLUIModule/qml/QMLUI.qml"));
    //QMLStatus(declarativeview_->status());
}

void QMLUIModule::QMLStatus(QDeclarativeView::Status qmlstatus)
{
    if (framework_->IsHeadless())
        return;
    if (qmlstatus == QDeclarativeView::Ready)
    {
        LogInfo("QDeclarativeView has loaded and created the QML component.");

        //qmluiproxy_->setVisible(true);
        //qmluiproxy_->setFocusPolicy(Qt::NoFocus);

        qmlui_ = dynamic_cast<QObject*>(declarativeview_->rootObject());
        QObject::connect(qmlui_, SIGNAL(exit()), this, SLOT(Exit()));

        QObject::connect(this, SIGNAL(giveQMLNetworkState(QVariant)), qmlui_, SLOT(networkstatechanged(QVariant)));

        QObject::connect(this, SIGNAL(giveQMLBatteryLevel(QVariant)), qmlui_, SLOT(batterylevelchanged(QVariant)));

        QObject::connect(this, SIGNAL(giveQMLUsingBattery(QVariant)), qmlui_,SLOT(usingbattery(QVariant)));

        QObject::connect(this, SIGNAL(giveQMLNetworkMode(QVariant)), qmlui_, SLOT(networkmodechanged(QVariant)));
    }
    else if (qmlstatus == QDeclarativeView::Null)
    {
        LogInfo("QDeclarativeView has no source set.");
    }
    else if (qmlstatus == QDeclarativeView::Loading)
    {
        LogInfo("QDeclarativeView is loading network data.");
    }
    else if (qmlstatus == QDeclarativeView::Error)
    {
        LogError("One or more errors has occurred.");
    }
    else
    {
        LogError("Unknown QDeclarativeView status!");
    }
}

void QMLUIModule::SceneAdded(const QString &name)
{
    Scene::ScenePtr scene = GetFramework()->Scene()->GetScene(name);
    connect(scene.get(), SIGNAL(ComponentAdded(Scene::Entity*, IComponent*, AttributeChange::Type)),
            SLOT(ComponentAdded(Scene::Entity*, IComponent*, AttributeChange::Type)));
    connect(scene.get(), SIGNAL(ComponentRemoved(Scene::Entity*, IComponent*, AttributeChange::Type)),
            SLOT(ComponentRemoved(Scene::Entity*, IComponent*, AttributeChange::Type)));

    //Scene::SceneManager *scenemanager = scene.get();

    renderer_ = framework_->GetService<Foundation::RenderServiceInterface>();

}

void QMLUIModule::ComponentAdded(Scene::Entity* entity, IComponent* comp, AttributeChange::Type change)
{
    if (comp->TypeName() == EC_Script::TypeNameStatic())
        connect(comp, SIGNAL(ScriptAssetChanged(ScriptAssetPtr)), this, SLOT(ScriptAssetChanged(ScriptAssetPtr)), Qt::UniqueConnection);
}

void QMLUIModule::ComponentRemoved(Scene::Entity *entity, IComponent *comp, AttributeChange::Type change)
{
    if (comp->TypeName() == EC_Script::TypeNameStatic())
        disconnect(comp, SIGNAL(ScriptAssetChanged(ScriptAssetPtr)), this, SLOT(ScriptAssetChanged(ScriptAssetPtr)));
}

void QMLUIModule::ScriptAssetChanged(ScriptAssetPtr newScript)
{
    EC_Script *sender = dynamic_cast<EC_Script*>(this->sender());
    assert(sender && "QMLUIModule::ScriptAssetChanged needs to be invoked from EC_Script!");
    if (!sender)
        return;
    QString scriptType = sender->type.Get().trimmed().toLower();
    if (scriptType != "qml" && scriptType.length() > 0)
        return; // The user enforced a foreign script type using the EC_Script type field.

    if (newScript->Name().endsWith(".qml") || scriptType == "qml") // We're positively using QML.
    {
        //Create just one declarativeview
        if (!view_created_)
            CreateQDeclarativeView();

        QString source;

        if (sender->getscriptRef().ref.startsWith("http", Qt::CaseInsensitive))
        {
            //Load ref from asset cache
            source = framework_->Asset()->GetAssetCache()->GetAbsoluteDataFilePath(sender->getscriptRef().ref);
            declarativeview_->setSource(QUrl(source));
        }
        else
        {
            //Load ref from under the bin file
            //TODO: Make finding file more spesific
            source = framework_->Asset()->RecursiveFindFile(".", sender->getscriptRef().ref);
            declarativeview_->setSource(QUrl(source));
        }
        LogInfo("QML source: "  + source.toStdString());
    }
}

void QMLUIModule::NetworkModeChanged(int mode)
{
    QVariant modee = (QVariant)mode;
    emit giveQMLNetworkMode(modee);
}

void QMLUIModule::Exit()
{
     framework_->Exit();
}

void QMLUIModule::NetworkStateChanged(int state)
{
    QVariant nstate = (QVariant)state;
    emit giveQMLNetworkState(nstate);
}

void QMLUIModule::BatteryLevelChanged(int batterylevel)
{
    QVariant blevel = (QVariant)batterylevel;
    emit giveQMLBatteryLevel(batterylevel);
}

void QMLUIModule::UsingBattery(bool uB)
{
    if (uB)
        emit giveQMLUsingBattery((QVariant)1);
    else
        emit giveQMLUsingBattery((QVariant)0);
}

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}

POCO_BEGIN_MANIFEST(IModule)
   POCO_EXPORT_CLASS(QMLUIModule)
POCO_END_MANIFEST

