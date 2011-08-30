// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "CallExtension.h"
#include "Call.h"
#include "Client.h"
#include "XMPPModule.h"
#include "UserItem.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{

QString CallExtension::extension_name_ = "Call";

CallExtension::CallExtension() :
    Extension(extension_name_),
    qxmpp_call_manager_(0)
{
}

CallExtension::~CallExtension()
{
    QString call;
    foreach(call, calls_.keys())
    {
        delete calls_[call];
        calls_.remove(call);
    }
}

void CallExtension::initialize(Client *client)
{
    qxmpp_call_manager_ = new QXmppCallManager();

    client_ = client;
    client_->getQxmppClient()->addExtension(qxmpp_call_manager_);
    framework_ = client_->getFramework();

    bool check;
    check = connect(qxmpp_call_manager_, SIGNAL(callReceived(QXmppCall*)), this, SLOT(handleCallReceived(QXmppCall*)));
    Q_ASSERT(check);
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

// callType is ignored becouse videochannel is not implemented in QXmpp 0.3.0
bool CallExtension::callUser(QString peerJid, QString peerResource, int callType)
{
    if(!client_ || !client_->getUser(peerJid))
        return false;

    UserItem* user_item = static_cast<UserItem*>(client_->getUser(peerJid));
    if(!user_item->getCapabilities(peerResource).contains("voice-v1"))
        return false;

    QString full_jid = peerJid + "/" + peerResource;

    QXmppCall *qxmpp_call = qxmpp_call_manager_->call(full_jid);

    if(!qxmpp_call)
        return false;

    /// \todo Check if we miss a signal becouse QXmppCall signals are suscribed inside XMPP::Call constructor
    Call *call = new Call(framework_, qxmpp_call);
    calls_.insert(peerJid, call);
    return true;
}

bool CallExtension::callUser(QString peerJid, QString peerResource, QStringList callType)
{
    int flags = 0;

    if(callType.size() == 0)
    {
        flags ^= 1;
    }
    else
    {
        if(callType.contains("Voice", Qt::CaseInsensitive))
            flags ^= 1;
        if(callType.contains("Video", Qt::CaseInsensitive))
            flags ^= 2;
    }

    return callUser(peerJid, peerResource, flags);
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
