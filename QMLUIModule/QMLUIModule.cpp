/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file
 *  @brief
 */

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "QMLUIModule.h"
#include "QMLWidget.h"

#include "UiServiceInterface.h"
#include "EventManager.h"
#include "ModuleManager.h"
#include "SceneAPI.h"
#include "UiAPI.h"




std::string QMLUIModule::type_name_static_ = "QMLUIModule";

QMLUIModule::QMLUIModule() :
    IModule(type_name_static_)

{
    //For QML-debugging
    QByteArray data = "1";
    qputenv("QML_IMPORT_TRACE", data);
}

QMLUIModule::~QMLUIModule()
{
}

void QMLUIModule::PreInitialize()
{
}

void QMLUIModule::Initialize()
{
}

void QMLUIModule::PostInitialize()
{

    mobilitymodule_ = framework_->GetModule<MobilityModule>();


    window_ = new QMLWidget();


    qmluiproxy_ = new UiProxyWidget(window_, Qt::Widget);
    framework_->Ui()->AddProxyWidgetToScene(qmluiproxy_);
    qmluiproxy_->setVisible(true);
    qmluiproxy_->setFocusPolicy(Qt::NoFocus);

    QObject *QMLUI = dynamic_cast<QObject*>(window_->rootObject());
    QObject::connect(QMLUI, SIGNAL(exit()), this, SLOT(Exit()));

    QObject::connect(mobilitymodule_, SIGNAL(networkStateChanged(MobilityModule::NetworkState)), this, SLOT(NetworkStateChanged(MobilityModule::NetworkState)));
    QObject::connect(this, SIGNAL(giveQMLNetworkState(QVariant)), QMLUI, SLOT(networkstatechanged(QVariant)));

    QObject::connect(mobilitymodule_, SIGNAL(batteryLevelChanged(int)), this, SLOT(BatteryLevelChanged(int)));
    QObject::connect(this, SIGNAL(giveQMLBatteryLevel(QVariant)), QMLUI, SLOT(batterylevelchanged(QVariant)));

    QObject::connect(mobilitymodule_, SIGNAL(usingBattery(bool)), this, SLOT(usingBattery(bool)));
    QObject::connect(this, SIGNAL(giveQMLUsingBattery(QVariant)), QMLUI,SLOT(usingbattery(QVariant)));

    QObject::connect(QMLUI, SIGNAL(setFocus(bool)), this, SLOT(setQMLUIFocus(bool)));

    QObject::connect(QMLUI, SIGNAL(loadxml()), this, SLOT(loadXML()));
    QObject::connect(this, SIGNAL(helloQML(QVariant)), QMLUI, SLOT(xmlfunction(QVariant)));

    emit giveQMLNetworkState((QVariant)mobilitymodule_->networkState());
    emit giveQMLBatteryLevel((QVariant)mobilitymodule_->batteryLevel());
    usingBattery(mobilitymodule_->usingBattery());

}

void QMLUIModule::Uninitialize()
{
    SAFE_DELETE(window_);
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

void QMLUIModule::setQMLUIFocus(bool focus)
{
    if (focus)
    {
        qmluiproxy_->setFocusPolicy(Qt::ClickFocus);
    }
    else
    {
        qmluiproxy_->setFocusPolicy(Qt::NoFocus);
    }
}

void QMLUIModule::loadXML()
{
    sceneMngr = framework_->Scene()->GetDefaultScene().get();

    //scenexml_ = sceneMngr->GetSceneXML(true, true);
    sceneMngr->SaveSceneXML(("testingxmlscene.xml"));
    emit helloQML("../../bin/testingxmlscene.xml");
}


void QMLUIModule::Exit()
{
     framework_->Exit();

     //sceneMngr = framework_->Scene()->GetDefaultScene().get();

     //scenexml_ = sceneMngr->GetSceneXML(true, true);
     //sceneMngr->SaveSceneXML(("testingxmlscene.xml"));
     //emit helloQML("../../testingxmlscene.xml");
}

void QMLUIModule::NetworkStateChanged(MobilityModule::NetworkState state)
{
    QVariant nstate = (QVariant)state;
    emit giveQMLNetworkState(nstate);
}

void QMLUIModule::BatteryLevelChanged(int batterylevel)
{
    QVariant blevel = (QVariant)batterylevel;
    emit giveQMLBatteryLevel(batterylevel);
}

void QMLUIModule::usingBattery(bool uB)
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

