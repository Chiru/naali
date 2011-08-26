// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "MucExtension.h"
#include "XMPPModule.h"

#include "qxmpp/QXmppMucManager.h"
#include "qxmpp/QXmppUtils.h"
#include "qxmpp/QXmppMessage.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{

QString MucExtension::extension_name_ = "Muc";

MucExtension::MucExtension(Foundation::Framework *framework, QXmppClient *client) :
    Extension(framework, client, extension_name_),
    qxmpp_muc_manager_(new QXmppMucManager())
{
    qxmpp_client_->addExtension(qxmpp_muc_manager_);

    bool check;

    check = connect(qxmpp_client_, SIGNAL(messageReceived(QXmppMessage)), this, SLOT(handleMessageReceived(QXmppMessage)));
    Q_ASSERT(check);

    check = connect(qxmpp_muc_manager_, SIGNAL(invitationReceived(QString,QString,QString)), this, SLOT(handleInvitationReceived(QString,QString,QString)));
    Q_ASSERT(check);

    check = connect(qxmpp_muc_manager_, SIGNAL(roomParticipantChanged(QString,QString)), this, SLOT(handleParticipantsChanged(QString,QString)));
    Q_ASSERT(check);

}

MucExtension::~MucExtension()
{
    QString room;
    foreach(room, rooms_.keys())
        leaveRoom(room);
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
                             + "Message from unkwnown Muc room (room = \"" + room_jid.toStdString() + "\")");
        return;
    }

    XMPPModule::LogDebug(extension_name_.toStdString()
                         + "Message (room = \"" + room_jid.toStdString()
                         + "\", sender = \"" + sender_jid.toStdString()
                         + "\", message =\"" + msg.toStdString() + "\"");

    emit messageReceived(room_jid, sender_jid, msg);

    return;
}

void MucExtension::handleInvitationReceived(const QString &room, const QString &inviter, const QString &reason)
{
    /// \todo check if we already are in the room

    XMPPModule::LogDebug(extension_name_.toStdString()
                         + "Invitation (room = \"" + room.toStdString()
                         + "\", inviter = \"" + inviter.toStdString()
                         + "\", reason =\"" + reason.toStdString() + "\"");

    emit invitationReceived(room, inviter, reason);

    return;
}

void MucExtension::handleParticipantsChanged(const QString &roomJid, const QString &nickName)
{
    if(!rooms_.keys().contains(roomJid))
    {
        XMPPModule::LogDebug(extension_name_.toStdString()
                             + "Unknown Muc room (room = \"" + roomJid.toStdString() + "\")");
        return;
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

bool MucExtension::joinRoom(QString room, QString nickname, QString password)
{
    if(qxmpp_muc_manager_->joinRoom(room, nickname, password))
    {
        MucRoom *muc_room = new MucRoom(room, nickname, password);
        rooms_.insert(room, muc_room);
        XMPPModule::LogDebug(extension_name_.toStdString()
                             + "Joined Muc room (room = \"" + room.toStdString() + "\")");
        emit roomAdded(room);
        return true;
    }
    return false;
}

bool MucExtension::leaveRoom(QString room)
{
    if(!rooms_.keys().contains(room))
        return false;

    if(qxmpp_muc_manager_->leaveRoom(room))
    {
        delete(rooms_[room]);
        rooms_.remove(room);
        XMPPModule::LogDebug(extension_name_.toStdString()
                             + "Left Muc room (room = \"" + room.toStdString() + "\")");
        emit roomRemoved(room, "Remove requested by user");
        return true;
    }
    return false;
}

QStringList MucExtension::getParticipants(QString room) const
{
    if(rooms_.keys().contains(room))
        return rooms_[room]->participants();
    return QStringList();
}

MucRoom::MucRoom(QString roomJid, QString userNickname, QString password) :
    jid_(roomJid),
    user_nickname_(userNickname),
    password_(password)
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
