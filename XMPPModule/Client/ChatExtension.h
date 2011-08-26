/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   ChatExtension.h
 *  @brief  Extension for XMPP:Client, provides p2p chat messaging support.
 */

#ifndef incl_XMPP_ChatExtension_h
#define incl_XMPP_ChatExtension_h

#include "Extension.h"

#include <QObject>
#include <QString>

class QXmppMessage;

namespace Foundation
{
    class Framework;
}

namespace XMPP
{

class ChatExtension : public Extension
{
    Q_OBJECT

public:
    ChatExtension(Foundation::Framework *framework, QXmppClient *client);
    virtual ~ChatExtension();

public slots:
    void sendMessage(QString receiver, QString message);

private slots:
    void handleMessageReceived(const QXmppMessage &message);

private:
    static QString extension_name_;

signals:
    void messageReceived(QString sender, QString message);
};

} // end of namespace: XMPP

#endif // incl_XMPP_ChatExtension_h
