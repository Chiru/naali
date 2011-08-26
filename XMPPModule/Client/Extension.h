/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   Extension.h
 *  @brief  Abstract class for XMPP:Client extensions.
 */

#ifndef incl_XMPP_Extension_h
#define incl_XMPP_Extension_h

#include "qxmpp/QXmppClient.h"

#include <QObject>
#include <QString>

typedef double f64;

namespace Foundation
{
    class Framework;
}

namespace XMPP
{

class Extension : public QObject
{
    Q_OBJECT

public:
    virtual ~Extension() {}
    explicit Extension(Foundation::Framework *framework, QXmppClient *client, const QString &name);
    virtual void Update(f64 frametime);

public slots:
    QString name() const;

protected:
    Foundation::Framework *framework_;
    QXmppClient *qxmpp_client_;
    const QString name_;
};

} // end of namespace: XMPP

#endif // incl_XMPP_Extension_h
