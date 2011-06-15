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
#include "QMLWidget.h"

#include "UiServiceInterface.h"
#include "EventManager.h"
#include "ModuleManager.h"
#include "SceneAPI.h"
#include "UiAPI.h"
#include "AssetAPI.h"




std::string QMLUIModule::type_name_static_ = "QMLUIModule";

QMLUIModule::QMLUIModule() :
    IModule(type_name_static_)

{
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
}

void QMLUIModule::PostInitialize()
{
    if (framework_->IsHeadless())
        return;

    window_ = new QMLWidget();
    QObject::connect(window_, SIGNAL(statusChanged(QDeclarativeView::Status)), this, SLOT(QMLStatus(QDeclarativeView::Status)));
    QMLStatus(window_->status());
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

void QMLUIModule::QMLStatus(QDeclarativeView::Status qmlstatus)
{
    if (framework_->IsHeadless())
        return;
    if (qmlstatus == QDeclarativeView::Ready)
    {
        LogInfo("QDeclarativeView has loaded and created the QML component.");
        qmluiproxy_ = new UiProxyWidget(window_, Qt::Widget);
        framework_->Ui()->AddProxyWidgetToScene(qmluiproxy_);
        qmluiproxy_->setVisible(true);
        qmluiproxy_->setFocusPolicy(Qt::NoFocus);

        QMLUI = dynamic_cast<QObject*>(window_->rootObject());
        QObject::connect(QMLUI, SIGNAL(exit()), this, SLOT(Exit()));

        QObject::connect(this, SIGNAL(giveQMLNetworkState(QVariant)), QMLUI, SLOT(networkstatechanged(QVariant)));

        QObject::connect(this, SIGNAL(giveQMLBatteryLevel(QVariant)), QMLUI, SLOT(batterylevelchanged(QVariant)));

        QObject::connect(this, SIGNAL(giveQMLUsingBattery(QVariant)), QMLUI,SLOT(usingbattery(QVariant)));

        QObject::connect(QMLUI, SIGNAL(setFocus(bool)), this, SLOT(SetQMLUIFocus(bool)));

        QObject::connect(this, SIGNAL(giveQMLNetworkMode(QVariant)), QMLUI, SLOT(networkmodechanged(QVariant)));
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
        LogInfo("One or more errors has occurred.");
    }
    else
    {
        LogInfo("Unknown QDeclarativeView status!");
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

