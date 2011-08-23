// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "VoiceCall.h"
#include "XMPPModule.h"
#include "qxmpp/QXmppRtpChannel.h"


#include "MemoryLeakCheck.h"

namespace XMPP
{
    VoiceCall::VoiceCall(Foundation::Framework* framework, QXmppCallManager* callManager) :
        framework_(framework),
        call_(0),
        call_manager_(callManager),
        active_(false)
    {
        bool ok;
        ok = connect(call_manager_, SIGNAL(callReceived(QXmppCall*)),
                     this, SLOT(handleCallReceived(QXmppCall*)));
        Q_ASSERT(ok);


    }

    VoiceCall::~VoiceCall()
    {
    }

    void VoiceCall::Update(f64 frametime)
    {
        handleOutboundVoice();
    }

    bool VoiceCall::acceptCall()
    {
        if(!call_)
            return false;

        call_->accept();
        return true;
    }

    void VoiceCall::handleCallReceived(QXmppCall *call)
    {
        XMPPModule::LogDebug("Call received from: " + call->jid().toStdString());

        call_ = call;
        peer_jid_ = call->jid();

        bool ok;
        ok = QObject::connect(call_, SIGNAL(stateChanged(QXmppCall::State)),
                              this, SLOT(handleCallStateChanged(QXmppCall::State)));
        Q_ASSERT(ok);

        ok = QObject::connect(call_, SIGNAL(connected()),
                              this, SLOT(handleCallConnected()));
        Q_ASSERT(ok);

        ok = QObject::connect(call_, SIGNAL(finished()),
                              this, SLOT(handleCallTerminated()));
        Q_ASSERT(ok);

    }

    bool VoiceCall::callUser(QString peerJid)
    {
        call_ = call_manager_->call(peerJid);
        peer_jid_ = call_->jid();

        if(!call_)
            return false;

        /// \todo move this and the other connect section to separate init function
        bool ok;
        ok = QObject::connect(call_, SIGNAL(stateChanged(QXmppCall::State)),
                              this, SLOT(handleCallStateChanged(QXmppCall::State)));
        Q_ASSERT(ok);

        ok = QObject::connect(call_, SIGNAL(connected()),
                              this, SLOT(handleCallConnected()));
        Q_ASSERT(ok);

        ok = QObject::connect(call_, SIGNAL(finished()),
                              this, SLOT(handleCallTerminated()));
        Q_ASSERT(ok);

        return true;
    }

    void VoiceCall::disconnect()
    {
        if(!call_)
            return;

        call_->disconnect();
    }

    void VoiceCall::handleCallTerminated()
    {
        call_ = 0;
        active_ = false;
        emit stateChanged(VoiceCall::DisconnectedState);
    }

    void VoiceCall::handleCallConnected()
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

    void VoiceCall::handleInboundVoice()
    {
        if(!call_)
            return;

        SoundBuffer buffer;
        QXmppRtpChannel *channel = call_->audioChannel();
        QByteArray data = channel->read(channel->bytesAvailable());

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

    void VoiceCall::handleOutboundVoice()
    {
        if(!call_ || !active_)
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


    void VoiceCall::handleCallStateChanged(QXmppCall::State state)
    {
        QString debug_info;
        switch(state)
        {
        case QXmppCall::ConnectingState:
            debug_info = "connecting";
            active_ = true;
            break;
        case QXmppCall::ActiveState:
            debug_info = "active";
            active_ = true;
            break;
        case QXmppCall::DisconnectingState:
            debug_info = "disconnecting";
            active_ = true;
            break;
        case QXmppCall::FinishedState:
            debug_info = "finished";
            active_ = false;
            break;
        default:
            break;
        }

        XMPPModule::LogInfo("Call with \"" + peer_jid_.toStdString() + "\" " + debug_info.toStdString());
    }

} // end of namespace: XMPP
