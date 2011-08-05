// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "AudioAPI.h"

#include "Client.h"
#include "qxmpp/QXmppVCardManager.h"
#include "qxmpp/QXmppRosterManager.h"
#include "qxmpp/QXmppPresence.h"
#include "qxmpp/QXmppUtils.h"

#include "qxmpp/QXmppMucIq.h"
#include "qxmpp/QXmppMucManager.h"


#include "MemoryLeakCheck.h"

namespace XMPP
{
    Client::Client(Foundation::Framework* framework, QXmppConfiguration &configuration) :
        framework_(framework),
        xmpp_client_(new QXmppClient()),
        xmpp_call_manager_(new QXmppCallManager()),
        xmpp_muc_manager_(new QXmppMucManager()),
        log_stream_(false),
        current_call_(0)
    {
        xmpp_client_->addExtension(xmpp_call_manager_);
        xmpp_client_->addExtension(xmpp_muc_manager_);

        xmpp_client_->logger()->setLoggingType(QXmppLogger::SignalLogging);

        connect(xmpp_client_, SIGNAL(messageReceived(const QXmppMessage&)), this, SLOT(HandleMessageReceived(const QXmppMessage&)));
        connect(xmpp_client_, SIGNAL(presenceReceived(const QXmppPresence&)), this, SLOT(HandlePresenceReceived(const QXmppPresence&)));
        connect(xmpp_client_, SIGNAL(connected()), this, SIGNAL(Connected()));
        connect(xmpp_client_, SIGNAL(disconnected()), this, SLOT(disconnect()));

        connect(xmpp_call_manager_, SIGNAL(callReceived(QXmppCall*)), this, SLOT(HandleIncomingCall(QXmppCall*)));

        connect(&xmpp_client_->rosterManager(), SIGNAL(rosterReceived()), this, SLOT(HandleRosterReceived()));
        connect(&xmpp_client_->rosterManager(), SIGNAL(rosterChanged(const QString&)), this, SLOT(HandleRosterChanged(const QString&)));
        connect(&xmpp_client_->rosterManager(), SIGNAL(presenceChanged(const QString&, const QString&)), this, SLOT(HandlePresenceChanged(const QString&, const QString&)));


        bool success = connect(xmpp_muc_manager_, SIGNAL(invitationReceived(QString,QString,QString)), this, SLOT(HandleMucInvite(QString,QString,QString)));
        Q_ASSERT(success);

        connect(&xmpp_client_->vCardManager(), SIGNAL(vCardReceived(const QXmppVCardIq&)), this, SLOT(HandleVCardReceived(const QXmppVCardIq&)));

        connect(QXmppLogger::getLogger(), SIGNAL(message(QXmppLogger::MessageType,QString)), this, SLOT(HandleLogMessage(QXmppLogger::MessageType,QString)));

        xmpp_client_->connectToServer(configuration, QXmppPresence::Available);
    }

    Client::~Client()
    {
        disconnect();
        /// \note handle terminating current call here
        SAFE_DELETE(xmpp_client_);
        SAFE_DELETE(current_call_);
    }

    void Client::disconnect()
    {
        /// \note check this logic
        //if(xmpp_client_->state() == QXmppClient::ConnectedState) // state() method only available in bleeding edge QXmpp
        xmpp_client_->disconnectFromServer();
        emit Disconnected();
    }


    void Client::HandleLogMessage(QXmppLogger::MessageType type, const QString &message)
    {
        QString prefix;
        switch(type)
        {
        case QXmppLogger::SentMessage:
            if(!log_stream_)
                return;
            prefix = ">>>";
            break;
        case QXmppLogger::ReceivedMessage:
            if(!log_stream_)
                return;
            prefix = "<<<";
            break;
        case QXmppLogger::WarningMessage:
            prefix = "!!!";
            break;
        default:
            prefix = "---";
            break;
        }

        XMPPModule::LogInfo(prefix.toStdString() + " " + message.toStdString());
    }

    QString Client::getHost()
    {
        return xmpp_client_->configuration().host();
    }

    void Client::setStreamLogging(bool state)
    {
        log_stream_ = state;
    }

    void Client::sendMessage(QString userJid, QString message)
    {
        xmpp_client_->sendMessage(userJid, message);
    }

    QObject* Client::getUser(QString userJid)
    {
        if(users_.contains(userJid))
            return dynamic_cast<QObject*>(users_[userJid]);
        return 0;
    }

    QStringList Client::getRoster()
    {
        return users_.keys();
    }

    void Client::HandleMucInvite(const QString &roomJid, const QString &inviterJid, const QString &reason)
    {
        XMPPModule::LogInfo("Muc request to join room \"" + roomJid.toStdString() + "\" from: \"" + inviterJid.toStdString() + "\" reason: \"" + reason.toStdString() + "\"");
        emit mucInvitationReceived(roomJid, inviterJid, reason);
    }

    QObject* Client::joinRoom(QString roomJid, QString nickname, QString password)
    {
        if(muc_rooms_.contains(roomJid))
            return getRoom(roomJid);

        if(xmpp_muc_manager_->joinRoom(roomJid, nickname, password))
        {
            MucRoom *muc_room = new MucRoom(xmpp_muc_manager_, roomJid, nickname, password);
            muc_rooms_.insert(roomJid, muc_room);
            return dynamic_cast<QObject*>(muc_room);
        }
        else
        {
            return 0;
        }
    }


    void Client::AcceptIncomingCall(QString callerJid)
    {
        if(current_call_)
            return;

        if(!xmpp_call_manager_->call(callerJid))
            return;

        current_call_ = new VoiceCall();

        /// \todo Connect call signals to current_call_ here

        emit CallStarted(callerJid);
    }

    void Client::HandleIncomingCall(QXmppCall *call)
    {
        emit IncomingCall(call->jid());
    }

    void Client::HandleRosterReceived()
    {
        XMPPModule::LogDebug(getHost().toStdString() + ": Received roster.");
        QStringList roster = xmpp_client_->rosterManager().getRosterBareJids();
        QString roster_user;
        foreach(roster_user, roster)
        {
            if(!users_.contains(roster_user))
            {
                QXmppRosterIq::Item item = xmpp_client_->rosterManager().getRosterEntry(roster_user);
                UserItem *user = new UserItem(item);
                users_[roster_user] = user;
                xmpp_client_->vCardManager().requestVCard(roster_user);
            }
        }
        emit RosterChanged();
    }

    void Client::HandleRosterChanged(const QString &userJid)
    {
        QXmppRosterIq::Item item = xmpp_client_->rosterManager().getRosterEntry(userJid);
        if(!users_.contains(userJid))
        {
            UserItem *user = new UserItem(item);
            users_[userJid] = user;
            /// \todo notify user added (should this logic be moved to a separate function?)
        }
        else
        {
            users_[userJid]->updateRosterItem(item);
        }
        emit RosterChanged();
    }

    void Client::HandleMessageReceived(const QXmppMessage &message)
    {
        QString jid = jidToBareJid(message.from());
        QString msg = message.body();

        // Consume GroupChat messages that also get sent here
        if(message.type() == QXmppMessage::GroupChat)
        {
            if(muc_rooms_.size() <= 0)
                return;

            QString room_jid = message.from().split("/").at(0);
            QString sender_jid = message.from().split("/").at(1);

            XMPPModule::LogDebug("Group message from room: " + room_jid.toStdString() + " from sender: " + sender_jid.toStdString());

            if(muc_rooms_.contains(room_jid))
                muc_rooms_[room_jid]->receiveMessage(sender_jid, msg);
            else
                XMPPModule::LogDebug("Received message from unknown chatroom: " + room_jid.toStdString());

            return;
        }

        emit PrivateMessageReceived(jid, msg);
    }

    QStringList Client::getRooms()
    {
        return muc_rooms_.keys();
    }

    QObject* Client::getRoom(QString roomJid)
    {
        if(muc_rooms_.contains(roomJid))
            return dynamic_cast<QObject*>(muc_rooms_[roomJid]);
        return 0;
    }

    void Client::HandlePresenceReceived(const QXmppPresence &presence)
    {
        /// \todo handle subscription requests also?
    }

    void Client::HandlePresenceChanged(const QString &userJid, const QString &resource)
    {
        if(xmpp_client_->configuration().jidBare() == userJid)
            return;

        if(!users_.contains(userJid))
            return;

        QMap<QString, QXmppPresence> presences = xmpp_client_->rosterManager().getAllPresencesForBareJid(userJid);
        QXmppPresence& presence = presences[resource];

        if(presence.type() == QXmppPresence::Available)
        {
            if(presence.vCardUpdateType() == QXmppPresence::VCardUpdateNone && !users_[userJid]->hasVCard())
            {
                xmpp_client_->vCardManager().requestVCard(userJid);
            }
        }

        users_[userJid]->updatePresence(resource, presence);
        emit PresenceChanged(presence.from());
    }

    void Client::HandleVCardReceived(const QXmppVCardIq& vcard)
    {
        QString userJid = vcard.from();
        XMPPModule::LogDebug(getHost().toStdString() + ": Received vCard from: " + userJid.toStdString());
        if(userJid == xmpp_client_->configuration().jidBare())
            return;

        if(!users_.contains(userJid))
            return;

        users_[userJid]->updateVCard(vcard);
        emit VCardChanged(userJid);
    }

    void Client::HandleSetPresence(QXmppPresence::Type presenceType)
    {

    }

    bool Client::addContact(QString userJid)
    {
        // Check if the given Jid is in proper format
        QRegExp re("^[^@]+@[^@]+$");
        if(userJid.isEmpty() || !re.exactMatch(userJid))
            return false;

        XMPPModule::LogDebug(getHost().toStdString() + ": Sending suscribe request to:" + userJid.toStdString());

        QXmppPresence subscribe;
        subscribe.setTo(userJid);
        subscribe.setType(QXmppPresence::Subscribe);

        return xmpp_client_->sendPacket(subscribe);
    }

} // end of namespace: XMPP
