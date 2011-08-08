// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "MucRoom.h"
#include "XMPPModule.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{
    MucRoom::MucRoom(QXmppMucManager *manager, const QString &roomJid, const QString &nickname, const QString &password) :
        manager_(manager),
        room_jid_(roomJid),
        nickname_(nickname),
        password_(password)
    {

        manager_->joinRoom(room_jid_, nickname_, password_);
    }

    MucRoom::~MucRoom()
    {
        manager_->leaveRoom(room_jid_);
        emit leftRoom();
    }

    void MucRoom::receiveMessage(const QString &userJid, const QString &message)
    {
        XMPPModule::LogDebug("Room \"" + room_jid_.toStdString() + "\" user: \"" + userJid.toStdString() + "\" message: \"" + message.toStdString() + "\"");
        emit messageReceived(userJid, message);
    }

    bool MucRoom::leaveRoom()
    {
        bool success = manager_->leaveRoom(room_jid_);
        if(success)
        {
            emit leftRoom();
            return true;
        }
        else
            return false;
    }

    bool MucRoom::sendMessage(QString message)
    {
        return manager_->sendMessage(room_jid_, message);
    }

    bool MucRoom::inviteUser(QString userJid)
    {
        return manager_->sendInvitation(room_jid_, userJid, QString("You have been requested to join conversation"));
    }
} // end of namespace: XMPP
