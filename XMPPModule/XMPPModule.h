/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   XMPPModule.h
 *  @brief  XMPP Communications with presence, text message and audio chat capabilities.
 */

#ifndef incl_XMPP_XMPPModule_h
#define incl_XMPP_XMPPModule_h

#include "IModule.h"
#include "ModuleLoggingFunctions.h"
#include "ForwardDefines.h"
#include "CoreStringUtils.h"
#include "qxmpp/QXmppConfiguration.h"

#include "XMPPModuleApi.h"

#include "AccountManager.h"
#include "Client.h"

#include <QObject>

namespace XMPP
{
class Client;
class AccountManager;

/**
 *  XMPP Communications support for tundra
 *
 *  Uses QXMPP library to provide presence, text and voice communications.
 *
 *  See usage example in <insert sample scene dir here>
 *
 *  Work in progress
 */
class XMPP_MODULE_API XMPPModule : public QObject, public IModule
{
    Q_OBJECT

public:
    /// Default constructor.
    XMPPModule();

    /// Destructor.
    ~XMPPModule();

    /// IModule override.
    void Load();

    /// IModule override.
    void Initialize();

    /// IModule override.
    void PostInitialize();

    /// IModule override.
    void Uninitialize();

    /// IModule override.
    void Update(f64 frametime);

    MODULE_LOGGING_FUNCTIONS

    /// Returns name of this module. Needed for logging.
    static const std::string &NameStatic() { return type_name_static_; }

public slots:
    QObject *getClient(QString host);
    QObject *newClient(QXmppConfiguration configuration);
    QObject *newClient(QString xmppServer, QString userJid, QString password);
    QObject *getAccountManager();

private slots:
    void HandleClientDisconnect(Client *client);


private:
    static std::string type_name_static_;
    AccountManager *account_manager_;
    QList<Client*> clients_;

};

} // end of namespace: XMPP


#endif // incl_XMPP_XMPPModule_h
