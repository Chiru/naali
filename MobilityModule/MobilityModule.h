#ifndef MOBILITYMODULE_H
#define MOBILITYMODULE_H

#include "IModule.h"
#include "ModuleLoggingFunctions.h"

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>
#include <QString>

#include <QSystemInfo>
#include <QSystemDeviceInfo>
#include <QSystemNetworkInfo>
#include <QSystemStorageInfo>
#include <QSystemScreenSaver>
#include <QSystemDisplayInfo>
#include <QNetworkAccessManager>


class MobilityModule : public QObject, public IModule
{
    Q_OBJECT

public:
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
    
    /// Returns the percentage of battery level
    int batteryLevel();
    
    /// Returns the status of the battery
    int batteryStatus();
    
    /// Returns whether the device is using wall or battery power
    int powerState();
    
    /// Returns whether the network is accessible
    bool networkAccessible();

private:
    
    /// Returns current date and time, used for primitive logging functions
    QString getTime();
    
    /// Type name of the module.
    static std::string type_name_static_;

    /// Input context for this module.
    boost::shared_ptr<InputContext> input_;

    /// "Framework" event category ID.
    event_category_id_t framework_category_;

    /// "NetworkState" event category ID.
    event_category_id_t network_category_;

    /// "Tundra" event category ID.
    event_category_id_t tundra_category_;
    
    QtMobility::QSystemInfo *system_info_;
    QtMobility::QSystemDeviceInfo *system_device_info_;
    QtMobility::QSystemNetworkInfo *system_network_info_;
    QtMobility::QSystemStorageInfo *system_storage_info_;
    QtMobility::QSystemScreenSaver *system_screen_saver_;
    QtMobility::QSystemDisplayInfo *system_display_info_;
    QNetworkAccessManager *network_access_manager_;
    
    //QtMobility::QSystemBatteryInfo *system_battery_info_;
    
    QFile log_file;
    QTextStream log_stream;
    QDate date;
    QTime time;
    
public slots:

    void batteryStatusHandler(QtMobility::QSystemDeviceInfo::BatteryStatus batteryStatus);
    void batteryLevelHandler(int batteryLevel);
    void powerStateHandler(QtMobility::QSystemDeviceInfo::PowerState powerState);
    void networkAccessibleHandler(QNetworkAccessManager::NetworkAccessibility networkAccessible);

signals:

    /// Emitted when battery status changes
    void batteryStatusChanged(QtMobility::QSystemDeviceInfo::BatteryStatus batteryStatus);
    
    /// Emitted when battery level changes
    void batteryLevelChanged(int batteryLevel);
    
    /// Emitted when power state changes from wall to battery or vice versa
    void powerStateChanged(QtMobility::QSystemDeviceInfo::PowerState powerState);
    
    /// Emitted when network accessibility changes
    void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility networkAccessible);
      
};

#endif // MOBILITYMODULE_H
