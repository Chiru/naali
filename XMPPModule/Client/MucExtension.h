/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   MucExtension.h
 *  @brief  Extension for XMPP:Client, provides multi-user chatroom support (XEP-0045).
 */

#ifndef incl_XMPP_MucExtension_h
#define incl_XMPP_MucExtension_h

#include "Extension.h"

#include <QObject>
#include <QString>
#include <QMap>

class QXmppMucManager;
class QXmppMessage;

namespace XMPP
{
class MucRoom;

class MucExtension : public Extension
{
    Q_OBJECT

public:
    MucExtension(Foundation::Framework *framework, QXmppClient *client);
    virtual ~MucExtension();

public slots:
    bool joinRoom(QString room, QString nickname, QString password = QString());
    bool leaveRoom(QString room);
    QStringList getRooms() const;
    QStringList getParticipants(QString room) const;

private slots:
    void handleMessageReceived(const QXmppMessage &message);
    void handleInvitationReceived(const QString &room, const QString &inviter, const QString &reason);
    void handleParticipantsChanged(const QString &roomJid, const QString &nickName);

private:
    static QString extension_name_;
    QXmppMucManager* qxmpp_muc_manager_;
    QMap<QString, MucRoom*> rooms_;

signals:
    void messageReceived(QString room, QString sender, QString message);
    void invitationReceived(QString room, QString from, QString reason);

    /// Emitted when room is added
    /// \note there's no proper way in qxmpp to detect if room join was succesful.
    ///       currently this signal gets emitted when join request was succesfully sent.
    void roomAdded(QString room);

    void roomRemoved(QString room, QString reason);
    void userJoinedRoom(QString room, QString user);
    void userLeftRoom(QString room, QString user);
};

class MucRoom
{
public:
    ~MucRoom();

    QString jid() const { return jid_; }
    QString userNickname() const { return user_nickname_; }
    QStringList participants() const { return participants_; }
    QString password() const { return password_; }

protected:
    void participantLeft(QString participant);
    void participantJoined(QString participant);

private:
    MucRoom(QString roomJid, QString userNickname, QString password);

    QString jid_;
    QString user_nickname_;
    QStringList participants_;
    QString password_;

    friend class MucExtension;
};

} // end of namespace: XMPP

#endif // incl_XMPP_MucExtension_h
