// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "Framework.h"
#include "XMPPModule.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{
std::string XMPPModule::type_name_static_ = "XMPPModule";

XMPPModule::XMPPModule() :
    IModule(type_name_static_),
    account_manager_(0)
{
}

XMPPModule::~XMPPModule()
{
    for(int i = 0; i < clients_.size(); i++)
    {
        SAFE_DELETE(clients_[i]);
    }
    clients_.clear();
    SAFE_DELETE(account_manager_);
}

void XMPPModule::Load()
{
    framework_->RegisterDynamicObject("xmpp", this);
    LogInfo("Initializing...");
}

void XMPPModule::Initialize()
{
}

void XMPPModule::PostInitialize()
{
    account_manager_ = new AccountManager(framework_);
    account_manager_->loadFromFile();
}

void XMPPModule::Uninitialize()
{
}

void XMPPModule::Update(f64 frametime)
{
}

QObject* XMPPModule::getClient(QString host)
{
    for(int i = 0; i < clients_.size(); i++)
    {
        if(QString::compare(clients_[i]->getHost(), host, Qt::CaseInsensitive) == 0)
            return dynamic_cast<QObject*>(clients_[i]);
    }
    return 0;
}

QObject* XMPPModule::newClient(QString xmppServer, QString userJid, QString password)
{
    QXmppConfiguration configuration;
    configuration.setHost(xmppServer);
    configuration.setJid(userJid);
    configuration.setPassword(password);
    return newClient(configuration);
}

QObject* XMPPModule::newClient(QXmppConfiguration configuration)
{
    Client *client = new Client(framework_, configuration);
    clients_.append(client);
    return dynamic_cast<QObject*>(client);
}

QObject* XMPPModule::getAccountManager()
{
    if(account_manager_)
        return dynamic_cast<QObject*>(account_manager_);
    return 0;
}

void XMPPModule::HandleClientDisconnect(Client *client)
{
    //SAFE_DELETE(client);
}

}// end of namespace: XMPP

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}

using namespace XMPP;

POCO_BEGIN_MANIFEST(IModule)
   POCO_EXPORT_CLASS(XMPPModule)
POCO_END_MANIFEST
