// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"

#include "DebugOperatorNew.h"

#include "Server.h"
#include "XMPPModule.h"
#include "PasswordChecker.h"
#include "RosterExtension.h"

#include "Framework.h"

#include "qxmpp/QXmppServer.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{

Server::Server(Foundation::Framework *framework) :
    qxmpp_server_(0),
    password_checker_(0),
    qxmpp_logger_(0)
{
    XMPPModule::LogInfo("Initializing Tundra XMPP server...");

    password_checker_ = new PasswordChecker(framework);
    roster_extension_ = new RosterExtension(framework);
    qxmpp_logger_ = new QXmppLogger();
    qxmpp_server_ = new QXmppServer();

    bool check = connect(qxmpp_logger_, SIGNAL(message(QXmppLogger::MessageType,QString)), this, SLOT(handleLogMessage(QXmppLogger::MessageType,QString)));
    Q_ASSERT(check);

    qxmpp_logger_->setLoggingType(QXmppLogger::SignalLogging);
    qxmpp_server_->setLogger(qxmpp_logger_);
    qxmpp_server_->setPasswordChecker(password_checker_);
    qxmpp_server_->setDomain(QHostInfo::localDomainName());

    qxmpp_server_->addExtension(roster_extension_);

    qxmpp_server_->listenForClients(); // Change port to be customizable

    XMPPModule::LogDebug("Tundra XMPP server initialized");
    XMPPModule::LogDebug("--Domain  " + qxmpp_server_->domain().toStdString());
}

Server::~Server()
{
    qxmpp_server_->close();
    delete qxmpp_server_;
}

void Server::handleLogMessage(QXmppLogger::MessageType type, const QString &message)
{
    // Make proper check for messagetypes etc.
    XMPPModule::LogDebug(message.toStdString());
}

} // end of namespace: XMPP
