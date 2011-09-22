// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "RosterExtension.h"

#include "Framework.h"
#include "TundraLogicModule.h"
#include "UserConnection.h"

#include "qxmpp/QXmppConstants.h"
#include "qxmpp/QXmppRosterIq.h"
#include "qxmpp/QXmppServer.h"
#include "qxmpp/QXmppServerPlugin.h"
#include "qxmpp/QXmppStream.h"
#include "qxmpp/QXmppServer.h"
#include "qxmpp/QXmppUtils.h"

#include <QDomElement>


namespace XMPP
{

RosterExtension::RosterExtension(Foundation::Framework *framework) // TODO: do we need to call extension constructor?
{
    TundraLogic::TundraLogicModule *tundra_logic = framework->GetModule<TundraLogic::TundraLogicModule>();
    tundra_server_ = tundra_logic->GetServer();
}

QStringList RosterExtension::discoveryFeatures() const
{
    return QStringList() << ns_roster;
}

bool RosterExtension::handleStanza(QXmppStream *stream, const QDomElement &element)
{
    Q_UNUSED(stream);

    if (element.attribute("to") != server()->domain())
        return false;

    const QString type = element.attribute("type");
    if (element.tagName() == "iq" && QXmppRosterIq::isRosterIq(element) && type == "get")
    {
        QXmppRosterIq request;
        request.parse(element);

        QXmppRosterIq response;
        response.setType(QXmppIq::Result);
        response.setId(request.id());
        response.setFrom(jidToBareJid(request.from()));
        response.setTo(request.from());

        TundraLogic::Server *tundra_server = tundra_server_.get();

        UserConnectionList users = tundra_server->GetAuthenticatedUsers();

        for (UserConnectionList::const_iterator iter = users.begin(); iter != users.end(); ++iter)
        {
            QString username = (*iter)->GetProperty("username");
            QString bare_jid;
            bare_jid = username;
            bare_jid.append("@");
            bare_jid.append(server()->domain());

            QXmppRosterIq::Item item;
            item.setBareJid(bare_jid);
            item.setName(username);
            item.setSubscriptionType(QXmppRosterIq::Item::Both);

            response.addItem(item);
        }

        server()->sendPacket(response);
        return true;
    }
    return false;
}

} // end of namespace: XMPP
