// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "Extension.h"

namespace XMPP
{

Extension::Extension(Foundation::Framework *framework, QXmppClient *client, const QString &name) :
    framework_(framework),
    qxmpp_client_(client),
    name_(name)
{
}

void Extension::Update(f64 frametime)
{
}

QString Extension::name() const
{
    return name_;
}

} // end of namespace: XMPP
