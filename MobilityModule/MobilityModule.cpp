/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   InWorldChatModule.cpp
 *  @brief  Stub implementation of the MobilityModule which is supposed to gather data from various
 *          sources and pass it on for other modules.
 */

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "ModuleManager.h"

#include "MobilityModule.h"

#include "ConsoleCommandServiceInterface.h"
#include "EventManager.h"

#include "MemoryLeakCheck.h"

QTM_USE_NAMESPACE

std::string MobilityModule::type_name_static_ = "Mobility";

MobilityModule::MobilityModule() :
    IModule(type_name_static_),
    framework_category_(0),
    network_category_(0)
{
}

MobilityModule::~MobilityModule()
{
}

void MobilityModule::PreInitialize()
{
}

void MobilityModule::Initialize()
{
	system_info_ = new QSystemInfo(this);
	system_device_info_ = new QSystemDeviceInfo(this);
	system_screen_saver_ = new QSystemScreenSaver(this);
	system_storage_info_ = new QSystemStorageInfo(this);
	system_network_info_ = new QSystemNetworkInfo(this);
	system_display_info_ = new QSystemDisplayInfo(this);
	network_access_manager_ = new QNetworkAccessManager(this);
	
	log_file.setFileName("./MobilityLog.txt");
	
	if(log_file.open( QIODevice::WriteOnly | QIODevice::Append ))
	{
	    log_stream.setDevice(&log_file);
	}
	
	// TODO: For testing purposes, remove these.
	batteryLevel();
	batteryStatus();
	powerState();
	networkAccessible();
	
	//system_battery_info_ = new QSystemBatteryinfo(this); // 1.2.0 ?
}

void MobilityModule::PostInitialize()
{
    framework_category_ = framework_->GetEventManager()->QueryEventCategory("Framework");
    tundra_category_ = framework_->GetEventManager()->QueryEventCategory("Tundra");
    
    connect(system_device_info_, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)), this, SLOT(batteryStatusHandler(QtMobility::QSystemDeviceInfo::BatteryStatus)));
    connect(system_device_info_, SIGNAL(batteryLevelChanged(int)), this, SLOT(batteryLevelHandler(int)));
    connect(system_device_info_, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)), this, SLOT(powerStateHandler(QtMobility::QSystemDeviceInfo::PowerState)));
}

bool MobilityModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data)
{
    return false;
}

void MobilityModule::Uninitialize()
{
}

void MobilityModule::Update(f64 frametime)
{
}

QString MobilityModule::getTime()
{
    // TODO: This is awful. Do it properly later.
    
    QString date_time = date.currentDate().toString();
    date_time.append(" ");
    date_time.append(time.currentTime().toString());
    
    return date_time;
}

void MobilityModule::batteryStatusHandler(QtMobility::QSystemDeviceInfo::BatteryStatus batteryStatus)
{
    log_stream << getTime() << " Battery status changed to: " << batteryStatus << "\n";
    emit batteryStatusChanged(batteryStatus);
}

void MobilityModule::batteryLevelHandler(int batteryLevel)
{
    log_stream << getTime() << " Battery level changed to: " << batteryLevel << "%\n";
    emit batteryLevelChanged(batteryLevel);
}

void MobilityModule::powerStateHandler(QtMobility::QSystemDeviceInfo::PowerState powerState)
{
    log_stream << getTime() << " Power state changed to: " << powerState << "\n";
    emit powerStateChanged(powerState);
}

void networkAccessibleHandler(QNetworkAccessManager::NetworkAccessibility networkAccessible)
{
}

int MobilityModule::batteryStatus()
{
    log_stream << getTime() << " Queried battery status: " << system_device_info_->batteryStatus() << "\n";
    return system_device_info_->batteryStatus();
}

int MobilityModule::batteryLevel()
{
    log_stream << getTime() << " Queried battery level: " << system_device_info_->batteryLevel() << "%\n";
    return system_device_info_->batteryLevel();
}

int MobilityModule::powerState()
{
    log_stream << getTime() << " Queried power state: " << system_device_info_->currentPowerState() << "\n";
    return system_device_info_->currentPowerState();
}

bool MobilityModule::networkAccessible()
{
    // TODO: Clean up. Figure out why networkAccessible() returns undetermined status.

    bool accessible;
    
    switch(network_access_manager_->networkAccessible())
    {
        case QNetworkAccessManager::Accessible:
            accessible = true;
            break;
        case QNetworkAccessManager::UnknownAccessibility:
            accessible = false;
            break;
        default:
            accessible = false;
            break;
    }
    
    if(accessible) log_stream << getTime() << " Queried network accessibility: Accessible" << "\n";
    else log_stream << getTime() << " Queried network accessibility: Unaccessible" << "\n";
    
    return accessible;
}

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}

POCO_BEGIN_MANIFEST(IModule)
   POCO_EXPORT_CLASS(MobilityModule)
POCO_END_MANIFEST
