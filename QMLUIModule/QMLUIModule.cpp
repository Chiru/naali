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
#include <QPinchGesture>

std::string QMLUIModule::type_name_static_ = "QMLUIModule";

QMLUIModule::QMLUIModule() :
    IModule(type_name_static_)

{
    view_created_ = false;
    camera_focused_on_entity = false;
    camera_moving = false;
    camera_saved = false;
    pinching_mode = false;
    qml_moving = false;
    camera_movement_timer_ = new QTimer();
    mouse_press_timer_ = new QTimer();
    camera_swipe_timer_ = new QTimer();
    scene_added = false;
    editing_mode = false;
    handle_mouse_events = true;
    camera_movement_timer_->setInterval(20);
    mouse_press_timer_->setInterval(700);
    mouse_press_timer_->setSingleShot(true);
    camera_swipe_timer_->setInterval(30);
    renderer_ = 0;
    camera_ = 0;
    context_ = 0;
    engine_ = 0;

    QObject::connect(camera_movement_timer_, SIGNAL(timeout()), this, SLOT(SmoothCameraMove()));
    QObject::connect(mouse_press_timer_, SIGNAL(timeout()), this, SLOT(SingleShot()));
    QObject::connect(camera_swipe_timer_, SIGNAL(timeout()), this, SLOT(CameraSwipe()));
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

    //Create a new input context that QMLUIModule will use to fetch input.
    input_ = framework_->Input()->RegisterInputContext("QMLUIInput", 100);


    // Listen on mouse input signals.
    connect(input_.get(), SIGNAL(MouseEventReceived(MouseEvent *)), this, SLOT(HandleMouseInputEvent(MouseEvent *)));
}

void QMLUIModule::Uninitialize()
{
}

void QMLUIModule::Update(f64 frametime)
{
    RESETPROFILER;
}



void QMLUIModule::CreateQDeclarativeView()
{
    //Prepare QDeclarativeView
    if (framework_->IsHeadless() || view_created_)
        return;

    view_created_ = true;

    declarativeview_ = new QDeclarativeView();
    engine_ = declarativeview_->engine();
    context_ = engine_->rootContext();
    context_->setContextProperty("pinchopacity", 0.0);
    context_->setContextProperty("movetextopacity", 0.0);
    context_->setContextProperty("pinchcenterx", 0);
    context_->setContextProperty("pinchcentery", 0);
    context_->setContextProperty("pinchx", 0);
    context_->setContextProperty("pinchy", 0);
    context_->setContextProperty("pinchxx", 0);
    context_->setContextProperty("pinchyy", 0);
    context_->setContextProperty("screenwidth", renderer_->GetWindowWidth());
    context_->setContextProperty("screenheight", renderer_->GetWindowHeight());
    context_->setContextProperty("entityname", "");
    QObject::connect(declarativeview_, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(QMLStatus(QDeclarativeView::Status)));

    declarativeview_->move(0,0);
    declarativeview_->setStyleSheet("QDeclarativeView {background-color: transparent;}");
    declarativeview_->setWindowState(Qt::WindowFullScreen);
    declarativeview_->setResizeMode(QDeclarativeView::SizeRootObjectToView);


    declarativeview_->setFocusPolicy(Qt::NoFocus);


    qmluiproxy_ = new UiProxyWidget(declarativeview_, Qt::Widget);

    framework_->Ui()->AddProxyWidgetToScene(qmluiproxy_);

    declarativeview_->setSource(QUrl::fromLocalFile("../QMLUIModule/qml/QMLUI.qml"));


}

void QMLUIModule::QMLStatus(QDeclarativeView::Status qmlstatus)
{
    if (framework_->IsHeadless())
        return;
    if (qmlstatus == QDeclarativeView::Ready)
    {
        LogInfo("QDeclarativeView has loaded and created the QML component.");

        qmluiproxy_->setVisible(true);
        qmluiproxy_->setFocusPolicy(Qt::NoFocus);



        qmlui_ = dynamic_cast<QObject*>(declarativeview_->rootObject());
        QObject::connect(qmlui_, SIGNAL(exit()), this, SLOT(Exit()));

        QObject::connect(qmlui_, SIGNAL(pinching(int)), this, SLOT(SetPinchingMode(int)));

        QObject::connect(qmlui_, SIGNAL(move(QString)), this, SLOT(MoveReceived(QString)));

        QObject::connect(qmlui_, SIGNAL(qmlmoving(int)), this, SLOT(SetQMLMoving(int)));

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

    renderer_ = framework_->GetService<Foundation::RenderServiceInterface>();

    scene_added = true;

    CreateQDeclarativeView();

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
            //BUG: This doesn't work
            source = framework_->Asset()->GetAssetCache()->GetDiskSource(sender->getscriptRef().ref);
            declarativeview_->setSource(QUrl(source));
        }
        LogInfo("QML source: "  + source.toStdString());
    }
}

void QMLUIModule::MoveReceived(QString direction)
{   
    emit Move(direction);
}

void QMLUIModule::SetPinchingMode(int i)
{
    if (i == 1)
    {
        pinching_mode = true;
        input_->SetTakeMouseEventsOverQt(true);
    }
    else
    {
        pinching_mode = false;
        input_->SetTakeMouseEventsOverQt(false);
    }
}

void QMLUIModule::SetQMLMoving(int i)
{
    if (i == 1)
        qml_moving = true;
    else
        qml_moving = false;
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

Scene::Entity* QMLUIModule::GetActiveCamera() const
{
    Scene::ScenePtr scene = GetFramework()->Scene()->GetDefaultScene();
    if (!scene)
    {
        //LogError("Failed to find active camera, default world scene wasn't setted.");
        return 0;
    }

    foreach(Scene::EntityPtr cam, scene->GetEntitiesWithComponent(EC_OgreCamera::TypeNameStatic()))
        if (cam->GetComponent<EC_OgreCamera>()->IsActive())
        {
            return cam.get();
        }

    //LogError("No active camera were found.");
    return 0;
}

void QMLUIModule::SingleShot()
{
    if ((move_start_x - last_pos_x) < 4 && (move_start_x - last_pos_x) > -4 && (move_start_y - last_pos_y) < 4 && (move_start_y - last_pos_y) > -4)
    {
        LongPress(move_start_x, move_start_y);
    }
    else
        return;
}

void QMLUIModule::LongPress(qreal x, qreal y)
{
    //Don't allow editing if camera is zoomed into entity
    if (camera_focused_on_entity)
        return;

    RaycastResult* result;
    if (renderer_)
    {
        result = renderer_->Raycast(x, y);

        if(result->entity_ && result->entity_->HasComponent("EC_Placeable") && !result->entity_->HasComponent("EC_Terrain"))
        {
            entity_to_edit_ = result->entity_;
            editing_mode = true;
            context_->setContextProperty("entityname", "Entity being moved: " + entity_to_edit_->GetName());
            context_->setContextProperty("movetextopacity", 1.0);
            StartDrag(x, y);
        }
    }
}

void QMLUIModule::StartDrag(int x, int y)
{
    move_start_x = x;
    move_start_y = y;
    last_pos_x = x;
    last_pos_y = y;
    float width = renderer_->GetWindowWidth();
    float height = renderer_->GetWindowHeight();
    float normalized_width = 1 / width;
    float normalized_height = 1 / height;
    last_abs_x = normalized_width * x;
    last_abs_y = normalized_height * y;
}

void QMLUIModule::SetHandleMouseEvents(bool value)
{
    handle_mouse_events = value;
}

void QMLUIModule::HandleMouseInputEvent(MouseEvent *mouse)
{
    if (!handle_mouse_events)
    {
        return;
    }
    if (!scene_added)
        return;

    if (camera_moving)
        return;



    if (GetActiveCamera() != 0 && GetActiveCamera()->GetName() != "FreeLookCamera")
    {
        camera_focused_on_entity = false;
        camera_moving = false;
        camera_saved = false;
        return;
    }

    if(mouse->eventType == MouseEvent::MousePressed && mouse->button == MouseEvent::LeftButton)
    {
        if (pinching_mode)
        {
            pinch_centerpoint = new QPoint(mouse->X(), mouse->Y());
            QPoint centerpoint;
            centerpoint.setX(pinch_centerpoint->x());
            centerpoint.setY(pinch_centerpoint->y());
            QPinchGesture *pinchGes = new QPinchGesture();
            pinchGes->setCenterPoint(centerpoint);
            pinchGes->setScaleFactor(1);
            pinchGes->setRotationAngle(0);
            last_scale = pinchGes->scaleFactor();
            last_rotation = pinchGes->rotationAngle();

            GestureEvent event;
            event.gesture = pinchGes;
            event.eventType = GestureEvent::GestureStarted;
            event.gestureType = Qt::PinchGesture;
            framework_->Input()->TriggerGestureEvent(event);
            context_->setContextProperty("pinchopacity", 0.7);
            context_->setContextProperty("pinchcenterx", mouse->X());
            context_->setContextProperty("pinchcentery", mouse->Y());
            context_->setContextProperty("pinchx", mouse->X());
            context_->setContextProperty("pinchy", mouse->Y());
            context_->setContextProperty("pinchxx", mouse->X());
            context_->setContextProperty("pinchyy", mouse->Y());
            delete pinchGes;
            return;
        }

        if (!camera_focused_on_entity && !camera_moving && !qml_moving)
        {
            mouse_press_timer_->start();

            StartDrag(mouse->X(), mouse->Y());

            if (camera_swipe_timer_->isActive())
                camera_swipe_timer_->stop();

            speed_x = 0;
        }
    }



    if(mouse->eventType == MouseEvent::MouseMove && mouse->IsButtonDown(MouseEvent::LeftButton))
    {
        if (pinching_mode)
        {
            QPoint centerpoint;
            centerpoint.setX(pinch_centerpoint->x());
            centerpoint.setY(pinch_centerpoint->y());
            QPoint *relpoint = new QPoint(mouse->X() - centerpoint.x(), mouse->Y() - centerpoint.y());

            if (relpoint->x() >= 0 && relpoint->y() < 0)
            {
                float hypo = sqrt(pow(relpoint->x(), 2) + pow(relpoint->y(), 2));
                float radian = acos((-1 * relpoint->y()) / hypo);
                float angle = radian * (180 / PI);
                QPinchGesture *pinchGes = new QPinchGesture();
                pinchGes->setCenterPoint(centerpoint);
                pinchGes->setScaleFactor(hypo);
                pinchGes->setRotationAngle(angle);
                pinchGes->setLastRotationAngle(last_rotation);
                pinchGes->setLastScaleFactor(last_scale);
                last_rotation = pinchGes->rotationAngle();
                last_scale = pinchGes->scaleFactor();

                GestureEvent event;
                event.gesture = pinchGes;
                event.eventType = GestureEvent::GestureUpdated;
                event.gestureType = Qt::PinchGesture;
                framework_->Input()->TriggerGestureEvent(event);
                delete pinchGes;
            }
            else if (relpoint->x() >= 0 && relpoint->y() >= 0)
            {
                float hypo = sqrt(pow(relpoint->x(), 2) + pow(relpoint->y(), 2));
                float radian = acos(relpoint->x() / hypo);
                float angle = radian * (180 / PI) + 90;
                QPinchGesture *pinchGes = new QPinchGesture();
                pinchGes->setCenterPoint(centerpoint);
                pinchGes->setScaleFactor(hypo);
                pinchGes->setRotationAngle(angle);
                pinchGes->setLastRotationAngle(last_rotation);
                pinchGes->setLastScaleFactor(last_scale);
                last_rotation = pinchGes->rotationAngle();
                last_scale = pinchGes->scaleFactor();

                GestureEvent event;
                event.gesture = pinchGes;
                event.eventType = GestureEvent::GestureUpdated;
                event.gestureType = Qt::PinchGesture;
                framework_->Input()->TriggerGestureEvent(event);
                delete pinchGes;
                //LogInfo(ToString(angle));

            }
            else if (relpoint->x() < 0 && relpoint->y() > 0)
            {
                float hypo = sqrt(pow(relpoint->x(), 2) + pow(relpoint->y(), 2));
                float radian = acos((-1 * relpoint->x()) / hypo);
                float angle = -1 * (radian * (180 / PI) + 90);
                QPinchGesture *pinchGes = new QPinchGesture();
                pinchGes->setCenterPoint(centerpoint);
                pinchGes->setScaleFactor(hypo);
                pinchGes->setRotationAngle(angle);
                pinchGes->setLastRotationAngle(last_rotation);
                pinchGes->setLastScaleFactor(last_scale);
                last_rotation = pinchGes->rotationAngle();
                last_scale = pinchGes->scaleFactor();

                GestureEvent event;
                event.gesture = pinchGes;
                event.eventType = GestureEvent::GestureUpdated;
                event.gestureType = Qt::PinchGesture;
                framework_->Input()->TriggerGestureEvent(event);
                delete pinchGes;
                //LogInfo(ToString(angle));

            }
            else if (relpoint->x() < 0 && relpoint->y() <= 0)
            {
                float hypo = sqrt(pow(relpoint->x(), 2) + pow(relpoint->y(), 2));
                float radian = acos((-1 * relpoint->y()) / hypo);
                float angle = -1 * radian * (180 / PI);
                QPinchGesture *pinchGes = new QPinchGesture();
                pinchGes->setCenterPoint(centerpoint);
                pinchGes->setScaleFactor(hypo);
                pinchGes->setRotationAngle(angle);
                pinchGes->setLastRotationAngle(last_rotation);
                pinchGes->setLastScaleFactor(last_scale);
                last_rotation = pinchGes->rotationAngle();
                last_scale = pinchGes->scaleFactor();

                GestureEvent event;
                event.gesture = pinchGes;
                event.eventType = GestureEvent::GestureUpdated;
                event.gestureType = Qt::PinchGesture;
                framework_->Input()->TriggerGestureEvent(event);
                delete pinchGes;
            }

            context_->setContextProperty("pinchx", mouse->X());
            context_->setContextProperty("pinchy", mouse->Y());
            context_->setContextProperty("pinchxx", pinch_centerpoint->x() - relpoint->x());
            context_->setContextProperty("pinchyy", pinch_centerpoint->y() - relpoint->y());

            delete relpoint;
            return;
        }

        if (editing_mode)
        {
            DragEntity(mouse->X(), mouse->Y());
        }
        if (!editing_mode && !camera_focused_on_entity && !camera_moving && !qml_moving)
        {
            TurnCamera(mouse->X(), mouse->Y());
            speed_x = mouse->RelativeX();

            //LogInfo("RelY: " + ToString(speed_y));
        }
    }

    if(mouse->eventType == MouseEvent::MouseReleased && mouse->button == MouseEvent::LeftButton)
    {
        if (pinching_mode)
        {
            context_->setContextProperty("pinchopacity", 0.0);
            return;
        }
        if (editing_mode)
        {
            context_->setContextProperty("entityname", "");
            context_->setContextProperty("movetextopacity", 0.0);
        }

        editing_mode = false;
        mouse_press_timer_->stop();
        if ((speed_x > 5 || speed_x < -5) && !camera_focused_on_entity && !camera_moving && !qml_moving)
        {
            if (speed_x < -20)
                    speed_x = -20;
            if (speed_x > 20)
                speed_x = 20;

            if (!camera_swipe_timer_->isActive())
                camera_swipe_timer_->start();
        }
    }

    if (mouse->eventType == MouseEvent::MouseDoubleClicked && mouse->button == MouseEvent::LeftButton)
    {
        if (pinching_mode)
            return;
        if (!editing_mode && !camera_focused_on_entity && !camera_moving && !qml_moving) //We don't allow zooming if we are in editing mode or already zoomed into an entity or camera is already moving
        {
            RaycastResult* result;
            if (renderer_)
            {
                result = renderer_->Raycast(mouse->X(), mouse->Y());

                if(result->entity_ && result->entity_->HasComponent("EC_Placeable") && !result->entity_->HasComponent("EC_Terrain"))
                {
                    camera_moving = true;
                    //LogInfo("Camera Moving = TRUE");
                    zoomed_into_this_entity_ = result->entity_;
                    FocusCameraOnEntity(result->entity_);
                }
            }
        }
        else if (camera_focused_on_entity && !editing_mode && !camera_moving && !qml_moving)
        {
                    emit ReturningCamera();
                    camera_moving = true;
                    FocusCameraOnEntity(zoomed_into_this_entity_);
        }
    }
}

void QMLUIModule::CameraSwipe()
{
    if (GetActiveCamera())
    {
        Scene::Entity *cameraent = GetActiveCamera();
        if (cameraent->GetName() != "FreeLookCamera")
            return;
        IComponent *iComponent = cameraent->GetComponent("EC_Placeable").get();
        EC_Placeable *camera_placeable = dynamic_cast<EC_Placeable*>(iComponent);
        Transform trans = camera_placeable->gettransform();
        if (speed_x < 0)
        {
            trans.rotation.z += speed_x / 5;
            speed_x += 0.5;
            camera_placeable->settransform(trans);
        }
        else if (speed_x > 0)
        {
            trans.rotation.z += speed_x / 5;
            speed_x -= 0.5;
            camera_placeable->settransform(trans);
        }
        else
        {
            camera_swipe_timer_->stop();
        }
    }
}

void QMLUIModule::TurnCamera(int x, int y)
{
    if (GetActiveCamera())
    {
        Scene::Entity *cameraent = GetActiveCamera();
        if (cameraent->GetName() != "FreeLookCamera")
            return;
        IComponent *iComponent = cameraent->GetComponent("EC_Placeable").get();
        EC_Placeable *camera_placeable = dynamic_cast<EC_Placeable*>(iComponent);
        Transform cameratransform = camera_placeable->gettransform();
        cameratransform.rotation.z += (x - last_pos_x) / 12;
        cameratransform.rotation.x += (y - last_pos_y) / 12;
        camera_placeable->settransform(cameratransform);
        last_pos_x = x;
        last_pos_y = y;
    }
}


void QMLUIModule::DragEntity(int x, int y)
{
    IComponent *iComponent = entity_to_edit_->GetComponent("EC_Placeable").get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    Transform entityPos = placeable->gettransform();
    float width = renderer_->GetWindowWidth();
    float height = renderer_->GetWindowHeight();
    float normalized_width = 1 / width;
    float normalized_height = 1/ height;
    float mouse_abs_x = normalized_width * x;
    float mouse_abs_y = normalized_height * y;

    float movedx = mouse_abs_x - last_abs_x;
    float movedy = mouse_abs_y - last_abs_y;

    if (GetActiveCamera())
    {
        Scene::Entity *cam = GetActiveCamera();

        iComponent =  cam->GetComponent("EC_OgreCamera").get();
        EC_OgreCamera *camera = dynamic_cast<EC_OgreCamera*>(iComponent);
        iComponent = cam->GetComponent("EC_Placeable").get();
        camera_ = dynamic_cast<EC_Placeable*>(iComponent);

        float fov = camera->GetVerticalFov();
        Vector3df campos = camera_->gettransform().position;
        Vector3df entpos = placeable->gettransform().position;
        float length = campos.getDistanceFrom(entpos);

        float worldwidth = (tan(fov/2) * length) * 2;
        float worldheight = (height*worldwidth) / width;

        float movefactor = width / height;

        float amountx = (worldwidth * movedx) * movefactor;
        float amounty = (worldheight * movedy) * movefactor;

        last_abs_x = mouse_abs_x;
        last_abs_y = mouse_abs_y;
        last_pos_x = x;
        last_pos_y = y;

        entityPos.position += camera_->GetOrientation() * Vector3df(amountx, -1 * amounty, 0);
        placeable->settransform(entityPos);
    }
}

void QMLUIModule::FocusCameraOnEntity(Scene::Entity *entity)
{
    if (entity->HasComponent("EC_Placeable"))
    {
        if (GetActiveCamera())
        {
            if (GetActiveCamera()->GetName() != "FreeLookCamera")
                return;
            IComponent *iComponent = GetActiveCamera()->GetComponent("EC_Placeable").get();
            camera_ = dynamic_cast<EC_Placeable*>(iComponent);
            if (!camera_saved)
            {
                original_camera_transform_ = camera_->gettransform();
                camera_saved = true;
            }
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
    float when_to_stop_difference_x = 1;
    float when_to_stop_difference_y = 1;
    float when_to_stop_difference_z = 1;
    int divider = 20; //Affects the speed of camera movement. Higher value -> Slower speed

    Vector3df target_position;
    target_position.x = 0;
    target_position.y = 90;
    target_position.z = 0;
    target_position = entity_to_focus_on_->GetRelativeVector(target_position);
    entity_transform.SetPos(entity_to_focus_on_->gettransform().position.x + target_position.x, entity_to_focus_on_->gettransform().position.y + target_position.y, entity_to_focus_on_->gettransform().position.z + target_position.z);

    if (!camera_focused_on_entity)
    {
        diff_x = entity_transform.position.x - camera_transform.position.x;

        if (diff_x > (-1 * when_to_stop_difference_x) && diff_x < when_to_stop_difference_x)
        {
            camx = true;
        }
        else
        {
            float diff = diff_x / divider;
            if (diff < 1 && diff > -1)
            {
                if (diff > 0)
                    diff = 1;
                else
                    diff = -1;
            }

            camera_transform.position.x += diff;
        }

        diff_y = entity_transform.position.y - camera_transform.position.y;

        if (diff_y > (-1 * when_to_stop_difference_y) && diff_y < when_to_stop_difference_y)
        {
            camy = true;
        }
        else
        {
            float diff = diff_y / divider;
            if (diff < 1 && diff > -1)
            {
                if (diff > 0)
                    diff = 1;
                else
                    diff = -1;
            }
            camera_transform.position.y += diff;
        }

        diff_z = entity_transform.position.z - camera_transform.position.z;

        if (diff_z > (-1 * when_to_stop_difference_z) && diff_z < when_to_stop_difference_z)
        {
            camz = true;
        }
        else
        {
            float diff = diff_z / divider;
            if (diff < 1 && diff > -1)
            {
                if (diff > 0)
                    diff = 1;
                else
                    diff = -1;
            }
            camera_transform.position.z += diff;
        }

        camera_->SetPosition(camera_transform.position);
        camera_->LookAt(entity_to_focus_on_->gettransform().position);
        //LogInfo("Diffx: " + ToString(diff_x) + " Diffy: " + ToString(diff_y) + " Diffz: " + ToString(diff_z));

        if (camx && camy && camz)
        {
            camera_->LookAt(entity_to_focus_on_->gettransform().position);
            camera_focused_on_entity = true;
            //LogInfo("Camera Focused On Entity = TRUE");
            camera_moving = false;
            //LogInfo("Camera Moving = FALSE");
            camera_movement_timer_->stop();
            emit CameraFocusedOnEntity(entity_transform.position, camera_transform.position);
        }
    }

    else
    {
        when_to_stop_difference_x = 1;
        when_to_stop_difference_y = 1;
        when_to_stop_difference_z = 1;
        diff_x = original_camera_transform_.position.x - camera_transform.position.x;

        if (diff_x > (-1 * when_to_stop_difference_x) && diff_x < when_to_stop_difference_x)
        {
            camx = true;
        }
        else
        {
            float diff = diff_x / divider;
            if (diff < 1 && diff > -1)
            {
                if (diff > 0)
                    diff = 1;
                else
                    diff = -1;
            }

            camera_transform.position.x += diff;
        }

        diff_y = original_camera_transform_.position.y - camera_transform.position.y;

        if (diff_y > (-1 * when_to_stop_difference_y) && diff_y < when_to_stop_difference_y)
        {
            camy = true;
        }
        else
        {
            float diff = diff_y / divider;
            if (diff < 1 && diff > -1)
            {
                if (diff > 0)
                    diff = 1;
                else
                    diff = -1;
            }

            camera_transform.position.y += diff;
        }

        diff_z = original_camera_transform_.position.z - camera_transform.position.z;

        if (diff_z > (-1 * when_to_stop_difference_z) && diff_z < when_to_stop_difference_z)
        {
            camz = true;
        }
        else
        {
            float diff = diff_z / divider;
            if (diff < 1 && diff > -1)
            {
                if (diff > 0)
                    diff = 1;
                else
                    diff = -1;
            }

            camera_transform.position.z += diff;
        }

        camera_->SetPosition(camera_transform.position);
        camera_->LookAt(entity_to_focus_on_->gettransform().position);
        //LogInfo("Diffx: " + ToString(diff_x) + " Diffy: " + ToString(diff_y) + " Diffz: " + ToString(diff_z));

        if (camx && camy && camz)
        {
            camera_->SetPosition(original_camera_transform_.position);
            camera_->LookAt(entity_to_focus_on_->gettransform().position);
            camera_focused_on_entity = false;
            //LogInfo("Camera Focused On Entity = FALSE");
            camera_saved = false;
            camera_moving = false;
            //LogInfo("Camera Moving = FALSE");
            camera_movement_timer_->stop();
        }
    }
}

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}

POCO_BEGIN_MANIFEST(IModule)
   POCO_EXPORT_CLASS(QMLUIModule)
POCO_END_MANIFEST

