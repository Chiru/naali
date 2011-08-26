// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "ChatExtension.h"
#include "XMPPModule.h"

#include "qxmpp/QXmppMessage.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{

QString ChatExtension::extension_name_ = "Chat";

ChatExtension::ChatExtension(Foundation::Framework *framework, QXmppClient *client) :
    Extension(framework, client, extension_name_)
{
    bool check;
    check = connect(qxmpp_client_, SIGNAL(messageReceived(QXmppMessage)), this, SLOT(handleMessageReceived(QXmppMessage)));
    Q_ASSERT(check);
}

ChatExtension::~ChatExtension()
{

}

void ChatExtension::handleMessageReceived(const QXmppMessage &message)
{
    if(message.type() == QXmppMessage::GroupChat)
        return;

    QString sender_jid = message.from();
    QString msg = message.body();

    XMPPModule::LogDebug(extension_name_.toStdString()
                         + "Message (sender = \"" + sender_jid.toStdString()
                         + "\", message =\"" + msg.toStdString() + "\"");

    emit messageReceived(sender_jid, msg);
}

void ChatExtension::sendMessage(QString receiver, QString message)
{
    qxmpp_client_->sendMessage(receiver, message);
}

} // end of namespace: XMPP
