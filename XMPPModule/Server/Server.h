/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   Server.h
 *  @brief  XMPP Server to use for inworld communications
 */

#ifndef incl_XMPP_Server_h
#define incl_XMPP_Server_h

#include "qxmpp/QXmppLogger.h"

#include <QObject>
#include <QHostInfo>

namespace Foundation
{
    class Framework;
}

class QXmppServer;
class QXmppLogger;

namespace XMPP
{
class PasswordChecker;
class RosterExtension;

/**
 *  Tundra XMPP server implementation for inworld communications purposes
 *
 *  Uses QXMPP library
 *
 *  Generates users' rosters on-the-fly based on current tundra server's userlist
 *
 *  Work in progress
 */
class Server : public QObject
{
Q_OBJECT;

public:
    Server(Foundation::Framework *framework);
    ~Server();

public slots:

private slots:
    void handleLogMessage(QXmppLogger::MessageType type, const QString& message);

private:
    PasswordChecker *password_checker_;
    QXmppServer *qxmpp_server_;
    QXmppLogger *qxmpp_logger_;
    RosterExtension *roster_extension_;
};

} // end of namespace: XMPP

#endif // incl_XMPP_Server_h
