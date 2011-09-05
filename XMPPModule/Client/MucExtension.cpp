// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "MucExtension.h"
#include "Client.h"
#include "XMPPModule.h"

#include "qxmpp/QXmppMucManager.h"
#include "qxmpp/QXmppUtils.h"
#include "qxmpp/QXmppMessage.h"
#include "qxmpp/QXmppPresence.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{

QString MucExtension::extension_name_ = "Muc";

MucExtension::MucExtension() :
    Extension(extension_name_),
    qxmpp_muc_manager_(new QXmppMucManager())
{
}

MucExtension::~MucExtension()
{
    QString room;
    foreach(room, rooms_.keys())
        leaveRoom(room);
}

void MucExtension::initialize(Client *client)
{
    client_ = client;
    framework_ = client_->getFramework();

    client_->getQxmppClient()->addExtension(qxmpp_muc_manager_);

    bool check;

    check = connect(client_->getQxmppClient(), SIGNAL(messageReceived(QXmppMessage)), this, SLOT(handleMessageReceived(QXmppMessage)));
    Q_ASSERT(check);

    check = connect(client->getQxmppClient(), SIGNAL(presenceReceived(QXmppPresence)), this, SLOT(handlePresenceReceived(QXmppPresence)));
    Q_ASSERT(check);

    check = connect(qxmpp_muc_manager_, SIGNAL(invitationReceived(QString,QString,QString)), this, SLOT(handleInvitationReceived(QString,QString,QString)));
    Q_ASSERT(check);

    check = connect(qxmpp_muc_manager_, SIGNAL(roomParticipantChanged(QString,QString)), this, SLOT(handleParticipantsChanged(QString,QString)));
    Q_ASSERT(check);
}

void MucExtension::handleRoomAdded(const QString &room, const QString nickname)
{
    MucRoom *muc_room = new MucRoom(room, nickname);
    rooms_.insert(room, muc_room);

    XMPPModule::LogDebug(extension_name_.toStdString()
                         + ": Joined Muc room (room = \"" + room.toStdString()
                         + "\", nickname = \"" + nickname.toStdString() + "\")");
    emit roomAdded(room, nickname);
}

void MucExtension::handleMessageReceived(const QXmppMessage &message)
{
    if(message.type() != QXmppMessage::GroupChat)
        return;

    QString room_jid = message.from().split("/").at(0);
    QString sender_jid = message.from().split("/").at(1);
    QString msg = message.body();

    if(!rooms_.keys().contains(room_jid))
    {
        XMPPModule::LogDebug(extension_name_.toStdString()
                             + ": Message from unkwnown Muc room (room = \"" + room_jid.toStdString() + "\")");
        return;
    }

    XMPPModule::LogDebug(extension_name_.toStdString()
                         + ": Message (room = \"" + room_jid.toStdString()
                         + "\", sender = \"" + sender_jid.toStdString()
                         + "\", message =\"" + msg.toStdString() + "\"");

    emit messageReceived(room_jid, sender_jid, msg);

    return;
}

void MucExtension::handleInvitationReceived(const QString &room, const QString &inviter, const QString &reason)
{
    /// \todo check if we already are in the room

    XMPPModule::LogDebug(extension_name_.toStdString()
                         + ": Invitation (room = \"" + room.toStdString()
                         + "\", inviter = \"" + inviter.toStdString()
                         + "\", reason =\"" + reason.toStdString() + "\"");

    emit invitationReceived(room, inviter, reason);

    return;
}

void MucExtension::handleParticipantsChanged(const QString &roomJid, const QString &nickName)
{
    if(!rooms_.keys().contains(roomJid))
    {
        handleRoomAdded(roomJid, "");
    }

    if(!rooms_[roomJid]->participants().contains(nickName))
    {
        rooms_[roomJid]->participantJoined(nickName);
        emit userJoinedRoom(roomJid, nickName);
    }
    else
    {
        QMap<QString, QXmppPresence> new_participants = qxmpp_muc_manager_->roomParticipants(roomJid);
        if(!new_participants.keys().contains(nickName))
        {
            rooms_[roomJid]->participantLeft(nickName);
            emit userLeftRoom(roomJid, nickName);
        }
    }
    return;
}

void MucExtension::handlePresenceReceived(const QXmppPresence &presence)
{
    QString from_domain = jidToDomain(presence.from());
    if(!from_domain.contains("conference"))
        return;

    QString room_jid = jidToBareJid(presence.from());
    QString nickname = jidToResource(presence.from());

    if(rooms_.keys().contains(room_jid))
        rooms_[room_jid]->setNickname(nickname);
    else
        handleRoomAdded(room_jid, nickname);
}

bool MucExtension::joinRoom(QString room, QString nickname, QString password)
{
    if(!client_)
        return false;

    if(rooms_.keys().contains(room))
        return false;

    return qxmpp_muc_manager_->joinRoom(room, nickname, password);
}

bool MucExtension::leaveRoom(QString room)
{
    if(!client_)
        return false;

    if(!rooms_.keys().contains(room))
        return false;

    if(qxmpp_muc_manager_->leaveRoom(room))
    {
        delete(rooms_[room]);
        rooms_.remove(room);
        XMPPModule::LogDebug(extension_name_.toStdString()
                             + ": Left Muc room (room = \"" + room.toStdString() + "\")");
        emit roomRemoved(room, "Remove requested by user");
        return true;
    }
    return false;
}

bool MucExtension::sendMessage(QString room, QString message)
{
    if(!client_)
        return false;

    if(!rooms_.keys().contains(room))
        return false;

    return qxmpp_muc_manager_->sendMessage(room, message);
}

bool MucExtension::invite(QString room, QString peerJid, QString reason)
{
    if(!client_)
        return false;

    if(!rooms_.keys().contains(room))
        return false;

    return qxmpp_muc_manager_->sendInvitation(room, peerJid, reason);
}

QStringList MucExtension::getParticipants(QString room) const
{
    if(rooms_.keys().contains(room))
        return rooms_[room]->participants();
    return QStringList();
}

QStringList MucExtension::getRooms() const
{
    return rooms_.keys();
}

MucRoom::MucRoom(QString roomJid, QString userNickname) :
    jid_(roomJid),
    user_nickname_(userNickname)
{
}

void MucRoom::participantJoined(QString participant)
{
    if(!participants_.contains(participant))
        participants_.append(participant);
}

void MucRoom::participantLeft(QString participant)
{
    for(int i = 0; i < participants_.size(); i++)
    {
        if(participants_[i] == participant)
        {
            participants_.removeAt(i);
            return;
        }
    }
}

} // end of namespace: XMPP
