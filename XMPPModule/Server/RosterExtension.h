/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   RosterPlugin.h
 *  @brief  Plugin for QXmppServer to handle rosters for single tundra XMPP server
 */

#ifndef incl_XMPP_RosterExtension_h
#define incl_XMPP_RosterExtension_h

#include <Server.h>

#include "qxmpp/QXmppServerExtension.h"

#include "boost/shared_ptr.hpp"
#include <QStringList>


namespace Foundation
{
    class Framework;
}

typedef boost::shared_ptr<TundraLogic::Server> TundraServerPtr;

namespace XMPP
{

class RosterExtension : public QXmppServerExtension
{
    Q_OBJECT
    Q_CLASSINFO("ExtensionName", "roster");

public:
    RosterExtension(Foundation::Framework *framework);

    QStringList discoveryFeatures() const;
    bool handleStanza(QXmppStream *stream, const QDomElement &element);

private:
    QStringList m_discoveryItems;
    TundraServerPtr tundra_server_;
};

} // end of namespace: XMPP

#endif // incl_XMPP_RosterExtension_h
