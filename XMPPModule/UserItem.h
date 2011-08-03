#ifndef incl_XMPP_UserItem_h
#define incl_XMPP_UserItem_h

#include "Framework.h"


#include <qxmpp/QXmppJingleIq.h>
#include <qxmpp/QXmppUtils.h>
#include <qxmpp/QXmppRosterIq.h>
#include <qxmpp/QXmppPresence.h>
#include <qxmpp/QXmppVCardIq.h>

#include <QObject>
#include <QMap>
#include <QByteArray>
#include <QFlags>

namespace XMPP
{

//! Represents single entry in user's roster
//!
//!
//! This class is contained within XMPP:Client don't use directly
class UserItem : public QObject
{
    Q_OBJECT

public:
    struct ResourceItem
    {
        bool available;
        QStringList capabilities; // QStringList for script friendliness (scripts don't mix well with enums)
    };

    UserItem(const QXmppRosterIq::Item &roster_item);
    ~UserItem();

    void updateRosterItem(const QXmppRosterIq::Item &item);
    void updatePresence(const QString &resource, const QXmppPresence &presence);
    void updateVCard(const QXmppVCardIq &vcard);

public slots:
    //! Get user's Jabber ID
    QString getJid() { return bare_jid_; }

    //! Get all the capabilities this user has
    QStringList getCapabilities();

    //! Get status of the user's vCard
    bool hasVCard() { return has_vcard_; }

    //! Number of clients connected to user's Jabber ID
    int resourceCount() { return resources_.size(); }

    QString getBirthday() { return birthday_; }
    QString getEmail() { return email_; }
    QString getFullName() { return full_name_; }
    QByteArray getPhoto() { return photo_; }
    QString getPhotoType() { return photo_type_; }
    QString getUrl() { return url_; }

private:
    void checkAvailability();

    QMap<QString, ResourceItem> resources_;
    QString bare_jid_;

    /// \todo shieeet man, store these in struct.. or not, whatever.
    QString birthday_;
    QString email_;
    QString full_name_;
    QByteArray photo_;
    QString photo_type_;
    QString url_;

    bool available_;
    bool has_vcard_;

signals:
    void AvailabilityChanged(bool availability);
};

} // end of namespace: XMPP

#endif // incl_XMPP_UserItem_h
