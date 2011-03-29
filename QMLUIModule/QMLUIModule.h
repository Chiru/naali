#ifndef incl_QMLUIModule_QMLUIModule_h
#define incl_QMLUIModule_QMLUIModule_h

#include <QtGui/QMainWindow>
#include "IModule.h"
#include "ModuleLoggingFunctions.h"
#include "SceneManager.h"

#include <QtDeclarative>

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

    // IModule override
    bool HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data);



    MODULE_LOGGING_FUNCTIONS

    // Returns the name of this module. Needed for logging.
    static const std::string &NameStatic() { return type_name_static_; }

private:
    /// Type name of the module.
    static std::string type_name_static_;

    /// Login window.
    QMLWidget *window_;

    /// Input context for this module.
    boost::shared_ptr<InputContext> input_;

    /// "Framework" event category ID.
    event_category_id_t framework_category_;

    /// "NetworkState" event category ID.
    event_category_id_t network_category_;

    /// "Tundra" event category ID.
    event_category_id_t tundra_category_;

    /// Are we currently connected to world.
    bool connected_;

    QByteArray scenexml_;
    Scene::SceneManager *sceneMngr;

private slots:

    void HandleKeyEvent(KeyEvent *key);

    void ProcessTundraLogin(const QMap<QString, QString> &data);

    void Exit();

    void qmlTest(UiServiceInterface *ui, QDeclarativeView *view);

signals:
    void Connect(const QMap<QString, QString> &login_data);
    void helloQML(QVariant text);

};


#endif // QMLUIMODULE_H
