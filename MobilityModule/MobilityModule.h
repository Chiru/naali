// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_MobilityModule_MobilityModule_h
#define incl_MobilityModule_MobilityModule_h

#include "IModule.h"
#include "ModuleLoggingFunctions.h"

#include <QObject>
#include <QString>
#include <QList>

#include <QSystemInfo>
#include <QSystemDeviceInfo>
#include <QSystemNetworkInfo>
#include <QSystemDisplayInfo>

#include <QNetworkSession>
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkInterface>

using namespace QtMobility;

//class MobilityModule;





class MobilityModule : public QObject, public IModule
{
    Q_OBJECT

public:
    enum DeviceFeature
    {
        BluetoothFeature = 0,
        CameraFeature = 1,
        FmradioFeature = 2,
        IrFeature = 3,
        LedFeature = 4,
        MemcardFeature = 5,
        UsbFeature = 6,
        VibFeature	= 7,
        WlanFeature	= 8,
        SimFeature	= 9,
        LocationFeature = 10,
        VideoOutFeature = 11,
        HapticsFeature = 12
    };

    enum NetworkState
    {
        StateInvalid = 0,
        StateNotAvailable = 1,
        StateConnecting = 2,
        StateConnected = 3,
        StateClosing = 4,
        StateDisconnected = 5,
        StateRoaming = 6
    };

    enum NetworkMode
    {
        ModeUnknown = 0,
        ModeEthernet = 1,
        ModeWLAN = 2,
        Mode2G = 3,
        ModeCDMA2000 = 4,
        ModeWCDMA = 5,
        ModeHSPA = 6,
        ModeBluetooth = 7,
        ModeWiMax = 8
    };

    enum ScreenState
    {
        ScreenUnknown = 0,
        ScreenOn = 1,
        ScreenDimmed = 2,
        ScreenOff = 3,
        ScreenSaver = 4
    };

    /// Mapping for QSystemDeviceInfo::Feature <-> MobilityModule::DeviceFeature
    typedef struct _featuremap {
        QSystemInfo::Feature feature;
        MobilityModule::DeviceFeature mfeature;
    }FEATUREMAP;

    /// Default constructor.
    MobilityModule();

    /// Destructor.
    ~MobilityModule();

    /// IModule override.
    void PreInitialize();

    /// IModule override.
    void Initialize();

    /// IModule override.
    void PostInitialize();

    /// IModule override.
    void Uninitialize();

    /// IModule override.
    void Update(f64 frametime);

    /// IModule override.
    bool HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data);

    MODULE_LOGGING_FUNCTIONS

    /// Returns name of this module. Needed for logging.
    static const std::string &NameStatic() { return type_name_static_; }

    /// \return Current battery level percentage (0-100)
    int batteryLevel();

    /// \return Is the system using battery or wall power (true for battery, false for wall)
    bool usingBattery();

    /// \return Boolean for battery critical state
    /// \note Always returns false when on battery power
    bool batteryCritical();

    /// \return Current network state as defined in MobilityModule::NetworkState
    MobilityModule::NetworkState networkState();

    /// \return Current network mode as defined in MobilityModule::NetworkMode
    MobilityModule::NetworkMode networkMode();

    /// \return Current network quality percentage (0-100)
    int networkQuality();

    /// Checks if platform has a feature available
    /// \param feature Feature defined in MobilityModule::DeviceFeature
    /// \return Boolean for availability of the given feature
    bool featureAvailable(MobilityModule::DeviceFeature feature);

    /// \return Current screen state as defined in MobilityModule::ScreenState
    MobilityModule::ScreenState screenState();

    /// \param Critical level for battery power in percentage (1-100)
    void setBatteryCriticalValue(int criticalValue);

private:

    /// Open network session to monitor connection state
    /// \note Contais work-around for bug in QNetworkConfigurationManager which lists interfaces as
    /// active devices. Once the bug in Qt is fixed this can be changed.
    /// \bug If the current session changes state to inactive and new connection is made with different
    /// network configuration, network_session_ becomes invalid and connection state monitoring fails.
    /// Bugs in Qt bearer API makes it difficult to switch to new configuration.
    void initNetworkSession();

    /// Type name of the module.
    static std::string type_name_static_;

    /// QSystemInfo, source of device capabilities related info
    QSystemInfo *system_info_;

    /// QSystemDeviceInfo, source of battery related information
    QSystemDeviceInfo *system_device_info_;

    /// QSystemNetworkInfo
    QSystemNetworkInfo *system_network_info_;

    /// QSystemDisplayInfo
    QSystemDisplayInfo *system_display_info_;
    
    /// QNetworkConfigurationManager, used to keep track of active network configurations
    QNetworkConfigurationManager *network_configuration_manager_;

    /// Map of available features
    QMap<MobilityModule::DeviceFeature, bool> features_;

    /// Current battery level
    int battery_level_;

    /// Current battery critical state
    bool battery_critical_;

    /// Value after which battery is considered to be in critical state (current default 20)
    int battery_critical_value_;

    /// Current power source (true for battery, false for wall)
    bool using_battery_power_;

    /// Current screen state
    ScreenState screen_state_;

    /// Current network mode
    NetworkMode network_mode_;

    /// Current network state
    NetworkState network_state_;

    /// Current network quality
    int network_quality_;

    /// Active network session
    QNetworkSession *network_session_;


public slots:

    /// Handler for battery level information
    void batteryLevelHandler(int batteryLevel);

    /// Handler for power source information
    void usingBatteryHandler(QSystemDeviceInfo::PowerState powerState);

    /// Handler for network state information
    void networkStateHandler(QNetworkSession::State networkState);

    /// Handler for screen state information
    /// \todo Implement when MCE daemon becomes part of MeeGo or QSystemDeviceInfo provides the required data.
    void screenStateHandler(int screenState);

    /// Handler for network mode information
    /// \note Partially implemented so we can query the mode with networkMode(),
    /// actual signal is not yet emitted
    /// \todo Implement when QSystemNetworkInfo is patched to work properly with ConnMan or
    /// NetworkConfigurationManager works properly on MeeGo.
    void networkModeHandler(QSystemNetworkInfo::NetworkMode networkMode);

    /// Used to circumvent the problem that QNetworkConfigurationManager can't determine
    /// the system online state reliably on linux platform
    void networkConfigurationChanged(const QNetworkConfiguration &networkConfig);

signals:

    /// Emitted when battery level changes
    /// \note Only emitted when using battery power
    void batteryLevelChanged(int batteryLevel);

    /// Emitted when battery level drops to critical
    /// \note Only emitted once when battery level drops below critical threshold
    /// \note Not emitted when on wall power (shouldn't happen anyways)
    void batteryLevelCritical();

    /// Emitted when power state changes from wall to battery or vice versa
    void usingBattery(bool state);

    /// Emitted when network status changes (offline/online/roaming...)
    /// \note Not tested on ConnMan backend. On NetworkManager, only signals Connected and Disconnected states.
    void networkStateChanged(MobilityModule::NetworkState networkState);

    /// Emitted when network mode changes
    /// \note Currently unimplemented
    void networkModeChanged(MobilityModule::NetworkMode networkMode);

    /// Emitted when network quality changes
    /// \note Currently unimplemented
    void networkQualityChanged(int networkQuality);

    /// Emitted when screen status changes (not implemented yet)
    /// \note Currently unimplemented
    void screenStateChanged(MobilityModule::ScreenState screenState);
};


#endif // incl_MobilityModule_MobilityModule_h
