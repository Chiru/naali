/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   PasswordChecker.h
 *  @brief  Password checker for tundra XMPP server
 */

#ifndef incl_XMPP_PasswordChecker_h
#define incl_XMPP_PasswordChecker_h

#include "UserConnection.h"

#include "qxmpp/QXmppIncomingClient.h"

#include <QObject>

namespace Foundation
{
    class Framework;
}

namespace TundraLogic
{
    class Server;
}

typedef std::list<UserConnection*> UserConnectionList;

namespace XMPP
{

class PasswordChecker : public QXmppPasswordChecker
{

public:
    PasswordChecker(Foundation::Framework *framework);
    QXmppPasswordChecker::Error checkPassword(const QString &username, const QString &password);
    bool getPassword(const QString &username, QString &password);
    bool hasGetPassword() const { return true; }

private:
    TundraLogic::Server *tundra_server_;

};

} // end of namespace: XMPP

#endif // incl_XMPP_PasswordChecker_h
