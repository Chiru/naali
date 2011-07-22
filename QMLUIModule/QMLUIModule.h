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

class QMLWidget;

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

    QDeclarativeView *parentview_;

    /*/// Input context for this module.
    boost::shared_ptr<InputContext> input_;

    /// "Framework" event category ID.
    event_category_id_t framework_category_;

    /// "NetworkState" event category ID.
    event_category_id_t network_category_;

    /// "Tundra" event category ID.
    event_category_id_t tundra_category_;*/

    /// Proxywidget for 2D UI
    UiProxyWidget *qmluiproxy_;

    /// QObject for connecting signals to QDeclarativeView
    QObject *qmlui_;

    void CreateQDeclarativeView();

    bool view_created_;

    InputContextPtr input_;

    EC_Placeable *camera_;

    EC_Placeable *moving_placeable_;

    ComponentPtr cameraptr_;

    Foundation::RenderServiceInterface *renderer_;

    bool camera_focused_on_entity;

    bool camera_moving;

    bool entity_movable;

    int move_start_x;

    int move_start_y;

    float last_pos_x;
    float last_pos_y;
    float last_abs_x;
    float last_abs_y;

    QTimer *camera_movement_timer_;



    Scene::Entity *entity_to_rotate_;

    Scene::Entity *entity_to_drag_;

    EC_Placeable *entity_to_focus_on_;

private slots:
    void SmoothCameraMove();

    Scene::Entity* GetActiveFreeLookCamera() const;

    Scene::Entity* GetActiveCamera() const;

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

    //void HandleGestureInputEvent(GestureEvent *event);

    void HandleMouseInputEvent(MouseEvent *mouse);

    void FocusCameraOnEntity(Scene::Entity *entity);

    void RotateEntityHorizontally(Scene::Entity *entity, float rotation);

    void StartDrag(MouseEvent *mouse);

    void DragEntity(Scene::Entity *entity, MouseEvent *mouse);


signals:

    /// Direct signals to QMLUI
    void giveQMLNetworkState(QVariant);
    void giveQMLBatteryLevel(QVariant);
    void giveQMLUsingBattery(QVariant);
    void giveQMLNetworkMode(QVariant);
};


#endif // QMLUIMODULE_H
