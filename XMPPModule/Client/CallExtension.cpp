// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "CallExtension.h"
#include "Call.h"
#include "XMPPModule.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{

QString CallExtension::extension_name_ = "Call";

CallExtension::CallExtension(Foundation::Framework *framework, QXmppClient *client) :
    Extension(framework, client, extension_name_),
    qxmpp_call_manager_(new QXmppCallManager())
{
    qxmpp_client_->addExtension(qxmpp_call_manager_);

    bool check;
    check = connect(qxmpp_call_manager_, SIGNAL(callReceived(QXmppCall*)), this, SLOT(handleCallReceived(QXmppCall*)));
    Q_ASSERT(check);
}

CallExtension::~CallExtension()
{

}

void CallExtension::Update(f64 frametime)
{
    QString call;
    foreach(call, calls_.keys())
        calls_[call]->Update(frametime);
}

bool CallExtension::acceptCall(QString peerJid)
{
    if(!calls_.keys().contains(peerJid) || calls_[peerJid]->state() != Call::RingingState)
        return false;

    return calls_[peerJid]->accept();
}

bool CallExtension::callUser(QString peerJid, int callType)
{
    // callType is ignored becouse videochannel is not implemented in QXmpp 0.3.0
    QXmppCall *qxmpp_call = qxmpp_call_manager_->call(peerJid);

    if(!qxmpp_call)
        return false;

    /// \todo Check if we miss a signal becouse QXmppCall signals are suscribed inside XMPP::Call constructor
    Call *call = new Call(framework_, qxmpp_call);
    calls_.insert(peerJid, call);
    return true;
}

bool CallExtension::disconnectCall(QString peerJid)
{
    if(!calls_.keys().contains(peerJid))
        return false;

    calls_[peerJid]->disconnect();
    return true;
}

QString CallExtension::getActiveCall()
{
    QString call;
    foreach(call, calls_.keys())
    {
        if(calls_[call]->state() == Call::ActiveState)
            return calls_[call]->peerJid();
    }
    return "";
}

bool CallExtension::setActiveCall(QString peerJid)
{
    if(!calls_.keys().contains(peerJid))
        return false;

    QString previous = getActiveCall();
    if(previous != "")
        calls_[previous]->suspend();
    calls_[peerJid]->resume();
}

void CallExtension::handleCallReceived(QXmppCall *qxmppCall)
{
    QString from_jid = qxmppCall->jid();

    XMPPModule::LogDebug(extension_name_.toStdString()
                         + "Incoming call (from = \"" + from_jid.toStdString() + "\")");

    Call *call = new Call(framework_, qxmppCall);
    calls_.insert(from_jid, call);

    emit callIncoming(from_jid);
}

void CallExtension::handleCallStateChanged(QXmppCall::State state)
{
    QXmppCall *call = qobject_cast<QXmppCall*>(sender());
    Q_ASSERT(call);

    switch(state)
    {
    case Call::RingingState:
    case Call::ConnectingState:
        break;
    case Call::ActiveState:
        emit activeCallChanged(call->jid());
        break;
    case Call::SuspendedState:
        break;
    case Call::DisconnectingState:
        break;
    case Call::FinishedState:
        delete call;
        calls_.remove(call->jid());
        emit callDisconnected(call->jid());
        break;
    }
}

} // end of namespace: XMPP
