#ifndef incl_XMPP_MucRoom_h
#define incl_XMPP_MucRoom_h

#include "qxmpp/QXmppUtils.h"
#include "qxmpp/QXmppMucManager.h"

#include <QStringList>

namespace XMPP
{

//! Represents single multi user chatroom
//!
//! This class is contained within XMPP:Client
class MucRoom : public QObject
{
    Q_OBJECT

public:
    MucRoom(QXmppMucManager *manager, const QString& roomJid, const QString& nickname, const QString& password = QString());
    ~MucRoom();
    void receiveMessage(const QString& userJid, const QString& message);

public slots:
    QString getUserNick() { return nickname_; }
    QStringList getParticipants() { return participants_; }
    QString getRoomJid() { return room_jid_; }

    bool leaveRoom();
    bool inviteUser(QString userJid);
    bool sendMessage(QString message);



private:
    QXmppMucManager *manager_;

    QString nickname_;
    QString room_jid_;
    QStringList participants_;
    QString password_;


private slots:

signals:

    void leftRoom();
    void messageReceived(QString userJid, QString message);


};

} // end of namespace: XMPP

#endif // incl_XMPP_MucRoom_h
