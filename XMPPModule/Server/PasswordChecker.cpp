// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "PasswordChecker.h"
#include "XMPPModule.h"

#include "Framework.h"
#include "TundraLogicModule.h"
#include <Server.h>

#include "MemoryLeakCheck.h"

namespace XMPP
{

PasswordChecker::PasswordChecker(Foundation::Framework *framework) :
    tundra_server_(0)
{
    TundraLogic::TundraLogicModule *tundra_logic = framework->GetModule<TundraLogic::TundraLogicModule>();
    tundra_server_ = tundra_logic->GetServer().get();
}

QXmppPasswordChecker::Error PasswordChecker::checkPassword(const QString &username, const QString &password)
{
    if(!tundra_server_)
        return QXmppPasswordChecker::TemporaryError;

    XMPPModule::LogDebug("Checking password for user: " + username.toStdString());

    UserConnectionList users = tundra_server_->GetAuthenticatedUsers();

    for (UserConnectionList::const_iterator iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->GetProperty("username") == username)
        {
            if((*iter)->GetProperty("password") == password)
                return QXmppPasswordChecker::NoError;
            else
                return QXmppPasswordChecker::AuthorizationError;
        }
    }
    return QXmppPasswordChecker::AuthorizationError;
}

bool PasswordChecker::getPassword(const QString &username, QString &password)
{
    if(!tundra_server_)
        return false;

    UserConnectionList users = tundra_server_->GetAuthenticatedUsers();

    for (UserConnectionList::const_iterator iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->GetProperty("username") == username)
        {
            password = (*iter)->GetProperty("password");
            return true;
        }
    }
    return false;
}

} // end of namespace: XMPP
