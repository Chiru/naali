#ifndef incl_XMPP_VoiceCall_h
#define incl_XMPP_VoiceCall_h

#include "qxmpp/QXmppCallManager.h"

#include <QObject>
#include <QThread>

namespace Foundation
{
    class Framework;
}

namespace XMPP
{
//! Handles single user-user voice call
//!
//! Passes audio through Tundra AudioAPI
//! \todo everything
class VoiceCall
{
public:
    VoiceCall();
    ~VoiceCall();
};
} // end of namespace: XMPP

#endif // incl_XMPP_VoiceCall_h
