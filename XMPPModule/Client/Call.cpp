// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "Call.h"
#include "XMPPModule.h"

#include "qxmpp/QXmppRtpChannel.h"
#include "qxmpp/QXmppJingleIq.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{
Call::Call(Foundation::Framework *framework, QXmppCall *call) :
    framework_(framework),
    call_(call)
{
    if(call->direction() == QXmppCall::IncomingDirection)
        state_ = Call::RingingState;
    else if(call->direction() == QXmppCall::OutgoingDirection)
        state_ = Call::ConnectingState;

    bool check;
    check = connect(call_, SIGNAL(stateChanged(QXmppCall::State)), this, SLOT(handleCallStateChanged(QXmppCall::State)));
    Q_ASSERT(check);

    check = connect(call_, SIGNAL(connected()), this, SLOT(handleCallConnected()));
    Q_ASSERT(check);

    check = connect(call_, SIGNAL(finished()), this, SLOT(handleCallTerminated()));
    Q_ASSERT(check);
}

Call::~Call()
{
    if(audio_channel_)
    {
        framework_->Audio()->Stop(audio_channel_);
        audio_channel_.reset();
    }
}

bool Call::accept()
{
    if(state_ != Call::RingingState)
        return false;

    call_->accept();
    return true;
}

void Call::disconnect()
{
    call_->disconnect();
    setState(Call::FinishedState);
}

bool Call::suspend()
{
    if(state_ == Call::FinishedState || state_ == Call::SuspendedState)
        return false;

    // QXmpp has no method of actually putting calls on hold like XEP-0167 defines
    // We'll just change the internal state of our call and discard received data.
    // We need to craft the messages ourselves unless this changes.

    state_ = Call::SuspendedState;
    emit stateChanged(state_);
}

void Call::Update(f64 frametime)
{
    UNREFERENCED_PARAM(frametime);
    handleOutboundVoice();
}

void Call::handleCallTerminated()
{
    setState(Call::FinishedState);
}

void Call::handleCallConnected()
{
    if(!call_)
        return;

    if(!framework_->Audio())
    {
        XMPPModule::LogError("Tundra sound API not initialized, cannot initialize voice call.");
        handleCallTerminated();
    }

    QXmppRtpChannel *channel = call_->audioChannel();
    bool ok;

    ok = QObject::connect(channel, SIGNAL(readyRead()),
                          this, SLOT(handleInboundVoice()));
    Q_ASSERT(ok);

    bool stereo; /// \todo change this to global property of the call
    if(channel->payloadType().channels() == 2)
        stereo = true;
    else
        stereo = false;

    /// \todo change to something proper and define as a class member
    int buffer_size = 16/8*channel->payloadType().clockrate()*200/1000;

    framework_->Audio()->StartRecording("", channel->payloadType().clockrate(), true, stereo, buffer_size);
}

void Call::handleInboundVoice()
{
    SoundBuffer buffer;
    QXmppRtpChannel *channel = call_->audioChannel();
    QByteArray data = channel->read(channel->bytesAvailable());

    // For now, just discard the data when on hold (wastes downlink, proper implementation pending)
    if(state_ == Call::SuspendedState)
        return;

    buffer.data.resize(data.size());
    memcpy(&buffer.data[0], data.data(), data.size());

    buffer.frequency = channel->payloadType().clockrate();
    buffer.is16Bit = true;
    if(channel->payloadType().channels() == 2)
        buffer.stereo = true;
    else
        buffer.stereo = false;

    if(!audio_channel_)
        audio_channel_ = framework_->Audio()->PlaySoundBuffer(buffer, SoundChannel::Voice);
    else
        framework_->Audio()->PlaySoundBuffer(buffer, SoundChannel::Voice, audio_channel_);

}

void Call::handleOutboundVoice()
{
    if(state_ != Call::ActiveState)
        return;

    QXmppRtpChannel *channel = call_->audioChannel();
    QByteArray buffer;

    int buffer_size = (channel->payloadType().clockrate() * channel->payloadType().channels() * (16 / 8) * 160) / 1000;

    while (framework_->Audio()->GetRecordedSoundSize() > buffer_size)
    {
        buffer.resize(buffer_size);
        char *data = buffer.data();
        int bytes = framework_->Audio()->GetRecordedSoundData(data, buffer_size);
        buffer.resize(bytes);

        call_->audioChannel()->write(buffer);
    }
}


void Call::handleCallStateChanged(QXmppCall::State state)
{
    /// \todo clean this up before final commit
    QString debug_info;
    switch(state)
    {
    case QXmppCall::ConnectingState:
        debug_info = "connecting";
        setState(Call::ConnectingState);
        break;
    case QXmppCall::ActiveState:
        debug_info = "active";
        setState(Call::ActiveState);
        break;
    case QXmppCall::DisconnectingState:
        debug_info = "disconnecting";
        setState(Call::DisconnectingState);
        break;
    case QXmppCall::FinishedState:
        debug_info = "finished";
        setState(Call::FinishedState);
        break;
    default:
        break;
    }

    XMPPModule::LogInfo("Call with \"" + peer_jid_.toStdString() + "\" " + debug_info.toStdString());
}

void Call::setState(Call::State state)
{
    state_ = state;
    emit stateChanged(state_);
}

} // end of namespace: XMPP
