// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "Extension.h"

namespace XMPP
{

Extension::Extension(const QString &name) :
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
