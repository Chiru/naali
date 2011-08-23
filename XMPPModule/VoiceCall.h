#ifndef incl_XMPP_VoiceCall_h
#define incl_XMPP_VoiceCall_h

#include "qxmpp/QXmppCallManager.h"

#include "Framework.h"
#include "AudioAPI.h"

#include <QObject>
#include <QThread>

namespace Foundation
{
    class Framework;
}

namespace XMPP
{
//! Acts as a wrapper to QXmppCall class
//!
//! Passes audio through Tundra AudioAPI
//! \todo everything
class VoiceCall : public QObject
{
        Q_OBJECT
public:
    VoiceCall(Foundation::Framework* framework, QXmppCallManager* callManager);
    ~VoiceCall();

    void Update(f64 frametime);

    enum State { ConnectingState = 0, ActiveState, DisconnectingState, DisconnectedState };

public slots:
    //! Establish a new call with user
    //! \param peerJid Jabber ID to connect the call with
    //! \return bool for succesfull call request
    //! \note The call is not started before remote party accepts it,
    //!       callStarted() is signaled after succesfully initializing the call
    bool callUser(QString peerJid);

    //! Get the Jabber ID for the other party of the conversation
    //! \return QString containing callees/callers Jabber ID
    QString getPeerJid();

    //! Accept incoming call
    //! \param callerJid jid to accept the call for.
    //! \note callStarted() is signaled after succesfully initializing the call
    bool acceptCall();

    //! Disconnect current call
    //! \return bool true for success
    void disconnect();


private slots:
    void handleCallReceived(QXmppCall* call);
    void handleCallTerminated();
    void handleCallConnected();
    void handleCallStateChanged(QXmppCall::State state);
    void handleInboundVoice();
    void handleOutboundVoice();

private:
    Foundation::Framework *framework_;
    SoundChannelPtr audio_channel_;
    QXmppCallManager* call_manager_;
    QXmppCall *call_;
    bool active_;
    QString peer_jid_;

signals:
    void stateChanged(VoiceCall::State state);

};
} // end of namespace: XMPP

#endif // incl_XMPP_VoiceCall_h
