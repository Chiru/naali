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

std::string QMLUIModule::type_name_static_ = "QMLUIModule";

QMLUIModule::QMLUIModule() :
    IModule(type_name_static_)

{
    view_created_ = false;
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
}

void QMLUIModule::Uninitialize()
{
    SAFE_DELETE(declarativeview_);
}

void QMLUIModule::Update(f64 frametime)
{
    RESETPROFILER;
}

// virtual
bool QMLUIModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data)
{    
    return false;
}

void QMLUIModule::CreateQDeclarativeView()
{
    //Prepare QDeclarativeView
    if (framework_->IsHeadless() || view_created_)
        return;

    view_created_ = true;

    declarativeview_ = new QDeclarativeView();
    QObject::connect(declarativeview_, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(QMLStatus(QDeclarativeView::Status)));

    declarativeview_->move(0,0);
    declarativeview_->setStyleSheet("QDeclarativeView {background-color: transparent;}");
    declarativeview_->setWindowState(Qt::WindowFullScreen);
    declarativeview_->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    declarativeview_->setFocusPolicy(Qt::NoFocus);

    qmluiproxy_ = new UiProxyWidget(declarativeview_, Qt::Widget);
    framework_->Ui()->AddProxyWidgetToScene(qmluiproxy_);

    QMLStatus(declarativeview_->status());
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

