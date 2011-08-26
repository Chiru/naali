// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "Client.h"
#include "XMPPModule.h"
#include "UserItem.h"
#include "Extension.h"

#include "AudioAPI.h"

#include "qxmpp/QXmppClient.h"
#include "qxmpp/QXmppReconnectionManager.h"
#include "qxmpp/QXmppVCardManager.h"
#include "qxmpp/QXmppRosterManager.h"
#include "qxmpp/QXmppMessage.h"
#include "qxmpp/QXmppUtils.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{
    Client::Client(Foundation::Framework* framework, QXmppConfiguration &configuration) :
        framework_(framework),
        xmpp_client_(new QXmppClient()),
        log_stream_(false)
    {
        xmpp_client_->logger()->setLoggingType(QXmppLogger::SignalLogging);

        // -----Client signals-----
        connect(xmpp_client_, SIGNAL(messageReceived(const QXmppMessage&)),
                this, SLOT(handleMessageReceived(const QXmppMessage&)));
        connect(xmpp_client_, SIGNAL(presenceReceived(const QXmppPresence&)),
                this, SLOT(handlePresenceReceived(const QXmppPresence&)));
        connect(xmpp_client_, SIGNAL(connected()),
                this, SIGNAL(connected()));
        connect(xmpp_client_, SIGNAL(disconnected()),
                this, SLOT(disconnect()));

        // -----Rostermanager signals-----
        connect(&xmpp_client_->rosterManager(), SIGNAL(rosterReceived()),
                this, SLOT(handleRosterReceived()));
        connect(&xmpp_client_->rosterManager(), SIGNAL(rosterChanged(const QString&)),
                this, SLOT(handleRosterChanged(const QString&)));
        connect(&xmpp_client_->rosterManager(), SIGNAL(presenceChanged(const QString&, const QString&)),
                this, SLOT(handlePresenceChanged(const QString&, const QString&)));

        // -----vCardmanager signals-----
        connect(&xmpp_client_->vCardManager(), SIGNAL(vCardReceived(const QXmppVCardIq&)),
                this, SLOT(handleVCardReceived(const QXmppVCardIq&)));

        // -----Logger signals-----
        connect(QXmppLogger::getLogger(), SIGNAL(message(QXmppLogger::MessageType,QString)),
                this, SLOT(handleLogMessage(QXmppLogger::MessageType,QString)));

        xmpp_client_->connectToServer(configuration, QXmppPresence::Available);
    }

    Client::~Client()
    {
        disconnect();
        SAFE_DELETE(xmpp_client_);
    }

    void Client::Update(f64 frametime)
    {
        Extension *extension;
        foreach(extension, extensions_)
            extension->Update(frametime);
    }

    QObject* Client::getExtension(QString extensionName)
    {
        for(int i = 0; i < extensions_.size(); i++)
        {
            if(extensions_[i]->name() == extensionName)
                return dynamic_cast<QObject*>(extensions_[i]);
        }
        return 0;
    }

    void Client::disconnect()
    {
        //if(xmpp_client_->state() == QXmppClient::ConnectedState) // state() method only available in bleeding edge QXmpp
        xmpp_client_->disconnectFromServer();
        emit disconnected();
    }


    void Client::handleLogMessage(QXmppLogger::MessageType type, const QString &message)
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

    void Client::handleRosterReceived()
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
        emit rosterChanged();
    }

    void Client::handleRosterChanged(const QString &userJid)
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
        emit rosterChanged();
    }

    void Client::handleMessageReceived(const QXmppMessage &message)
    {
    }

    void Client::handlePresenceReceived(const QXmppPresence &presence)
    {
        /// \todo handle subscription requests also?
    }

    void Client::handlePresenceChanged(const QString &userJid, const QString &resource)
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
        emit presenceChanged(presence.from());
    }

    void Client::handleVCardReceived(const QXmppVCardIq& vcard)
    {
        QString userJid = vcard.from();
        XMPPModule::LogDebug(getHost().toStdString() + ": Received vCard from: " + userJid.toStdString());
        if(userJid == xmpp_client_->configuration().jidBare())
            return;

        if(!users_.contains(userJid))
            return;

        users_[userJid]->updateVCard(vcard);
        emit vCardChanged(userJid);
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
