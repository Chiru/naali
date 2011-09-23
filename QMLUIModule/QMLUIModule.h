/**
*  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
*  @file QMLUIModule.h
*  @brief QMLUIModule is used for showing a 2D overlay QML UI
*/

#ifndef incl_QMLUIModule_QMLUIModule_h
#define incl_QMLUIModule_QMLUIModule_h

#include "IModule.h"
#include "ModuleLoggingFunctions.h"
#include "SceneManager.h"
#include "UiProxyWidget.h"
#include "EC_Script.h"
#include "ScriptAsset.h"
#include "InputFwd.h"

#include "EC_Placeable.h"
#include "EC_Mesh.h"


#include <QtDeclarative>
#include <QTimer>

class KeyEvent;
class InputContext;

/*
 * WORK IN PROGRESS
 * Module for testing QML
 */

class QMLUIModule : public QObject, public IModule
{
    Q_OBJECT

public:
    // Default constructor
    QMLUIModule();

    //QMLUIModule(QWidget *parent = 0);

    // Destructor
    ~QMLUIModule();

    // IModule override
    void PreInitialize();

    // IModule override
    void Initialize();

    // IModule override
    void PostInitialize();

    // IModule override
    void Uninitialize();

    // IModule override
    void Update(f64 frametime);

    /*// IModule override
    bool HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data);*/

    MODULE_LOGGING_FUNCTIONS

    // Returns the name of this module. Needed for logging.
    static const std::string &NameStatic() { return type_name_static_; }

private:
    /// Type name of the module.
    static std::string type_name_static_;

    /// 2D UI widget.
    QDeclarativeView *declarativeview_;

    /// Input context for this module.
    boost::shared_ptr<InputContext> input_;

    /// Proxywidget for 2D UI
    UiProxyWidget *qmluiproxy_;

    /// QObject for connecting signals to QDeclarativeView
    QObject *qmlui_;

    QDeclarativeEngine *engine_;

    QDeclarativeContext *context_;

    void CreateQDeclarativeView();

    bool view_created_;

    EC_Placeable *camera_;

    EC_Placeable *moving_placeable_;

    ComponentPtr cameraptr_;

    Foundation::RenderServiceInterface *renderer_;

    bool camera_focused_on_entity;

    bool camera_moving;

    bool camera_saved;

    bool scene_added;

    bool editing_mode;

    bool qml_moving;

    bool pinching_mode;

    bool handle_mouse_events;

    QPoint *pinch_centerpoint;

    int move_start_x;

    int move_start_y;

    int mouse_pos_x;
    int mouse_pos_y;

    float last_pos_x;
    float last_pos_y;
    float last_abs_x;
    float last_abs_y;

    float speed_x;
    float speed_y;

    float last_scale;
    float last_rotation;

    QTimer *camera_movement_timer_;

    QTimer *mouse_press_timer_;

    QTimer *camera_swipe_timer_;

    Transform original_camera_transform_;

    Scene::Entity *entity_to_edit_;

    Scene::Entity *zoomed_into_this_entity_;

    EC_Placeable *entity_to_focus_on_;

private slots:
    void SmoothCameraMove();

    Scene::Entity* GetActiveCamera() const;

    void SingleShot();

    void SetPinchingMode(int i);

    void SetQMLMoving(int i);

public slots:

    /// Exits the application
    void Exit();

    /// Sends network state to QMLUI (for mobility module)
    void NetworkStateChanged(int);

    /// Sends battery level to QMLUI (for mobility module)
    void BatteryLevelChanged(int);

    /// Sends network mode to QMLUI (for mobility module)
    void NetworkModeChanged(int);

    /// Sends using battery QMLUI (for mobility module)
    void UsingBattery(bool);

    /// Handles changes in QML-status
    void QMLStatus(QDeclarativeView::Status);

    //! New component has been added to scene.
    void ComponentAdded(Scene::Entity* entity, IComponent* comp, AttributeChange::Type change);

    //! Component has been removed from scene.
    void ComponentRemoved(Scene::Entity* entity, IComponent* comp, AttributeChange::Type change);

    void ScriptAssetChanged(ScriptAssetPtr newScript);

    void SceneAdded(const QString &name);

    void HandleMouseInputEvent(MouseEvent *mouse);

    void FocusCameraOnEntity(Scene::Entity *entity);

    void StartDrag(int x, int y);

    void DragEntity(int x, int y);

    void LongPress(qreal x, qreal y);

    void TurnCamera(int x, int y);

    void CameraSwipe();

    void MoveReceived(QString direction);

    void SetHandleMouseEvents(bool value);

signals:

    void CameraFocusedOnEntity(Vector3df centerpoint, Vector3df camerapos);
    void ReturningCamera();
    void Move(QString direction);

    /// Direct signals to QMLUI
    void giveQMLNetworkState(QVariant);
    void giveQMLBatteryLevel(QVariant);
    void giveQMLUsingBattery(QVariant);
    void giveQMLNetworkMode(QVariant);

};


#endif // QMLUIMODULE_H
