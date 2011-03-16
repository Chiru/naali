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
    LogInfo(Name() + " initializing...");

    system_info_ = new QSystemInfo(this);
    system_device_info_ = new QSystemDeviceInfo(this);
    //system_network_info_ = new QSystemNetworkInfo(this);
    //system_display_info_ = new QSystemDisplayInfo(this);

    network_configuration_manager_ = new QNetworkConfigurationManager();

    initNetworkSession();

    // Query platform features from QtMobility
    features_.clear();
    for(int x = 0; x < 13; x++)
    {
        features_.append(system_info_->hasFeatureSupported((QSystemInfo::Feature)x));
    }

    // Set initial values for mobility related info by triggering the correspondig slots
    battery_critical_ = false;
    usingBatteryHandler(system_device_info_->currentPowerState());
    batteryLevelHandler(system_device_info_->batteryLevel());
    //screenStateHandler();

    connect(system_device_info_, SIGNAL(batteryLevelChanged(int)), this, SLOT(batteryLevelHandler(int)));
    connect(system_device_info_, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)), this, SLOT(usingBatteryHandler(QSystemDeviceInfo::PowerState)));
    connect(network_configuration_manager_, SIGNAL(configurationChanged(QNetworkConfiguration)), this, SLOT(networkConfigurationChanged(QNetworkConfiguration)));
}

void MobilityModule::PostInitialize()
{
    framework_category_ = framework_->GetEventManager()->QueryEventCategory("Framework");
    tundra_category_ = framework_->GetEventManager()->QueryEventCategory("Tundra");
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

void MobilityModule::batteryLevelHandler(int batteryLevel)
{
    // Nothing gets emitted unless we are running on battery power
    if(using_battery_power_)
    {
        if(batteryLevel <= BATTERYCRITICALVALUE && !battery_critical_)
        {
            battery_critical_ = true;
            LogWarning("Battery state critical");
            emit batteryLevelCritical();
        }
        emit batteryLevelChanged(batteryLevel);
    }

    battery_level_ = batteryLevel;
    LogInfo("Battery level changed to: " + ToString(battery_level_));
}

void MobilityModule::usingBatteryHandler(QSystemDeviceInfo::PowerState powerState)
{
    if(powerState == QSystemDeviceInfo::BatteryPower)
    {
        using_battery_power_ = true;
        LogInfo("Power source changed to: Battery");
        batteryLevelHandler(system_device_info_->batteryLevel()); // Call batteryLevelHandler() to emit current batterylevel and to set battery_critical_ to correct state
    }
    else
    {
        using_battery_power_ = false;
        battery_critical_ = false;
        LogInfo("Power source changed to: Wall");
    }

    emit usingBattery(using_battery_power_);
}

void MobilityModule::networkStateHandler(QNetworkSession::State networkState)
{
    QString connected_string_;

    switch(networkState)
    {
    case QNetworkSession::Invalid:
        connected_string_ = "Invalid";
    case QNetworkSession::NotAvailable:
        connected_string_ = "NotAvailable";
        break;
    case QNetworkSession::Connecting:
        connected_string_ = "Connecting";
        break;
    case QNetworkSession::Connected:
        connected_string_ = "Connected";
        break;
    case QNetworkSession::Closing:
        connected_string_ = "Closing";
        break;
    case QNetworkSession::Disconnected:
        connected_string_ = "Disconnected";
        break;
    case QNetworkSession::Roaming:
        connected_string_ = "Roaming";
        break;
    default:
        connected_string_ = "Unknown";
        break;
    }

    LogInfo("Network state changed to: " + connected_string_.toStdString());

    // Straight cast from QNetworkSession::State to MobilityModule::NetworkState, this needs to be handled differently if either is modified.
    network_state_ = (MobilityModule::NetworkState)networkState;

    /*if(networkState == QNetworkSession::Connected)
        initNetworkSession();*/

    emit networkStateChanged(network_state_);
}


void MobilityModule::screenStateHandler(int screenState)
{
    // Unimplemented, see header for details.
}

void MobilityModule::networkModeHandler(QSystemNetworkInfo::NetworkMode networkMode)
{
    // Temporary solution, query the network mode from active network configuration.

    if(network_session_)
    {
        LogInfo("Network mode changed to: " + network_session_->configuration().bearerTypeName().toStdString());
        network_mode_ = (MobilityModule::NetworkMode)network_session_->configuration().bearerType();
    }
    else
    {
        LogInfo("Network mode changed to: Unknown");
        network_mode_ = MobilityModule::ModeUnknown;
    }

    emit networkModeChanged((MobilityModule::NetworkMode)network_mode_);

}

void MobilityModule::networkConfigurationChanged(const QNetworkConfiguration &networkConfig)
{
    if(networkConfig.state() == QNetworkConfiguration::Active) initNetworkSession();
}


int MobilityModule::batteryLevel()
{
    return battery_level_;
}

bool MobilityModule::usingBattery()
{
    return using_battery_power_;
}

bool MobilityModule::batteryCritical()
{
    return battery_critical_;
}

MobilityModule::NetworkState MobilityModule::networkState()
{
    return network_state_;
}

MobilityModule::NetworkMode MobilityModule::networkMode()
{
    return network_mode_;
}

MobilityModule::ScreenState MobilityModule::screenState()
{
    return MobilityModule::ScreenUnknown;
}

int MobilityModule::networkQuality()
{
    /// \todo Change this when actual implementation of network
    return 100;
}

void MobilityModule::initNetworkSession()
{
    // QNetworkConfigurationManager counts interfaces as active configurations (atleast with NetworkManager and Connman backends), these need to be removed.
    QList<QNetworkConfiguration> configs_ = network_configuration_manager_->allConfigurations(QNetworkConfiguration::Active);

    foreach(const QNetworkInterface iface, QNetworkInterface::allInterfaces())
    {
        for(int x = 0; x < configs_.size(); x++)
        {
            if(QString::compare(configs_.at(x).name(), iface.humanReadableName(), Qt::CaseInsensitive) == 0)
                configs_.removeAt(x);
        }
    }


    // There's a configuration in active state, set QNetworkSession to track it.
    if(configs_.size() > 0)
    {
        network_session_ = new QNetworkSession(configs_.at(0));
        connect(network_session_, SIGNAL(stateChanged(QNetworkSession::State)), SLOT(networkStateHandler(QNetworkSession::State)));

        LogInfo("Active network configuration found, network session attached to: " + configs_.at(0).name().toStdString());
        if(configs_.size() > 1) LogDebug("Found multiple active configurations.");

        networkStateHandler(network_session_->state());
        networkModeHandler(QSystemNetworkInfo::UnknownMode);
    }
    // No active configuration found. The possibility to connect to a user defined access point can be implemented here.
    else if(configs_.size() == 0)
    {
        LogWarning("No active network configuration found, network state information disabled.");
    }
}


bool MobilityModule::featureAvailable(MobilityModule::DeviceFeature feature)
{
    if(features_.size() <= (int)feature)
        return false;
    return features_.at((int)feature);
}



extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}


POCO_BEGIN_MANIFEST(IModule)
   POCO_EXPORT_CLASS(MobilityModule)
POCO_END_MANIFEST
