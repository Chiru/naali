#ifndef incl_XMPP_Client_h
#define incl_XMPP_Client_h

#include "XMPPModule.h"

#include "VoiceCall.h"
#include "UserItem.h"
#include "MucRoom.h"

#include "qxmpp/QXmppClient.h"
#include "qxmpp/QXmppCallManager.h"
#include "qxmpp/QXmppMessage.h"
#include "qxmpp/QXmppJingleIq.h"
#include "qxmpp/QXmppUtils.h"
#include "qxmpp/QXmppPresence.h"
#include "qxmpp/QXmppUtils.h"
#include "qxmpp/QXmppReconnectionManager.h"
#include "qxmpp/QXmppRosterIq.h"
#include "qxmpp/QXmppVCardIq.h"
#include "qxmpp/QXmppLogger.h"
#include "qxmpp/QXmppStanza.h"
#include "qxmpp/QXmppMucManager.h"

#include <QObject>
#include <QThread>

namespace Foundation
{
    class Framework;
}

namespace XMPP
{

//! Represents a single connection to a XMPP Server
//!
//! Provides interface for sending and receiveing messages
//! and voice calls. Also keeps track of users' presence
//! status' and capabilities
//!
//! Uses QXmpp library for connection handling
class Client : public QObject
{
    Q_OBJECT
public:
    Client(Foundation::Framework* framework, QXmppConfiguration &configuration);
    ~Client();

public slots:
    //! Accept call request from Jabber ID
    void AcceptIncomingCall(QString callerJid);

    //! End active voice call
    void EndCall();

    //! Set own presence status
    //! \param QString
    void SetPresence(QString presenceType);

    //! Connect to a XMPP Server, script friendly overload
    //! \param userJid User's Jabber ID
    //! \param userPassword User's password
    //! \param xmppServer XMPP Server (host:port)
    void Connect(QString userJid, QString userPassword, QString xmppServer);

    //! Get host associated with this connection
    //! \return QString current host (host)
    QString getHost();

    //! Get UserItem
    //! \param userJid Jabber ID for the user
    QObject* getUser(QString userJid);

    //! Get multi user chatroom pointer
    //! \param roomName name of the muc room (room@service)
    //! \return QObject pointer for room if found, null pointer for room not found
    QObject* getRoom(QString roomName);

    //! Get available muc rooms
    //! \return QStringList containing available muc rooms
    QStringList getRooms();

    //! Get current roster
    //! \return QStringList containing known Jabber ID's
    QStringList getRoster();

    //! Join multi user chatroom
    //! \param roomJid name of the muc room (room@service)
    //! \param nickname user's nickname in the room
    //! \param password optional room password
    //! \return QObject pointer on success (pointer to old room if already joined), null pointer if join unsuccessfull
    QObject* joinRoom(QString roomJid, QString nickname, QString password = QString());

    //! Send message
    //! \param userJid Jabber ID the message is sent to
    //! \param message self explanatory
    void sendMessage(QString userJid, QString message);

    //! Disconnect from server
    //! \note Requesting a disconnect destroys this Client object
    void disconnect();

    //! Add contact to roster
    //! \param userJid Jabber ID to be added to roster
    //! \return bool describing if the request was succesfully sent
    bool addContact(QString userJid);

    //! Set logging of the XML stream on/off
    //! \param state Boolean for on/off
    void setStreamLogging(bool state);

private slots:
    void HandleIncomingCall(QXmppCall *call);
    void HandleCallStateChanged(QXmppCall::State state);
    void HandleMessageReceived(const QXmppMessage &message);
    void HandlePresenceChanged(const QString& userJid, const QString& resource);
    void HandlePresenceReceived(const QXmppPresence& presence);
    void HandleSetPresence(QXmppPresence::Type presenceType);
    void HandleRosterReceived();
    void HandleRosterChanged(const QString& userJid);
    void HandleVCardReceived(const QXmppVCardIq& vcard);
    void HandleLogMessage(QXmppLogger::MessageType type, const QString& message);
    void HandleMucInvite(const QString& roomJid, const QString& inviterJid, const QString& reason);

private:
    QXmppClient *xmpp_client_;
    QXmppCallManager *xmpp_call_manager_;
    QXmppMucManager *xmpp_muc_manager_;
    QXmppConfiguration *current_configuration_;
    VoiceCall *current_call_;
    QMap<QString, UserItem*> users_;    /// \todo do we need to use QMap when UserJid's can be fetched from UserItems?
    QMap<QString, MucRoom*> muc_rooms_;
    Foundation::Framework* framework_;
    bool log_stream_;

    //! Connect to a XMPP Server using QXmppConfiguration
    //! \param configuration Configuration containing connection details
    void ConnectToServer(const QXmppConfiguration& configuration);

signals:
    //! Signals incoming call from other jabber user,
    //! must be accepted with AcceptIncomingCall(QString CallerJid)
    void IncomingCall(QString CallerJid);

    //! Signals accepted call succesfully started
    void CallStarted(QString CallerJid);

    //! Signals active call has ended
    void CallEnded(QString CallerJid);

    //! Forwards incoming private message
    void PrivateMessageReceived(QString UserJid, QString Message);

    //! Forwards incoming multi user chatroom message
    //void mucMessageReceived(QString roomJid, QString userNick, QString message);

    //! Forwards invitation to join multi user chatroom
    void mucInvitationReceived(QString roomJid, QString inviterJid, QString reason);

    //! Signals changes in current roster
    void RosterChanged();

    //! Signals changes in users presence
    //! Can indicate that capabilities were received
    void PresenceChanged(QString UserJid);

    void VCardChanged(QString UserJid);

    //! Signals disconnect by request
    void Disconnected();

    //! Signals connected status
    //! \note This signal gets emitted when the underlying QXmppClient signals connected state
    void Connected();

};

} // end of namespace: XMPP

#endif // incl_XMPP_Client_h
