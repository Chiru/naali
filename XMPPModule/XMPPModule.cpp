// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "XMPPModule.h"
#include "Client/Client.h"
#include "Client/AccountManager.h"
#include "Server/Server.h"
#include "Server.h"

#include "Framework.h"
#include "TundraLogicModule.h"
#include "Server.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{
std::string XMPPModule::type_name_static_ = "XMPP";

XMPPModule::XMPPModule() :
    IModule(type_name_static_),
    account_manager_(0),
    server_(0)
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
    SAFE_DELETE(server_);
}

void XMPPModule::Load()
{
    framework_->RegisterDynamicObject("xmpp", this);
}

void XMPPModule::Initialize()
{
    account_manager_ = new AccountManager(framework_);
    account_manager_->loadFromFile();
}

void XMPPModule::PostInitialize()
{
    bool is_server = false;

    if(framework_)
    {
        TundraLogic::TundraLogicModule *tundra_logic = framework_->GetModule<TundraLogic::TundraLogicModule>();
        TundraLogic::Server *tundra_server = tundra_logic->GetServer().get();

        if(tundra_server)
            is_server = tundra_server->IsRunning() | tundra_server->IsAboutToStart();
    }

    if(is_server)
        server_ = new Server(framework_);
}

void XMPPModule::Uninitialize()
{
}

void XMPPModule::Update(f64 frametime)
{
    for(int i = 0; i < clients_.size(); i++)
    {
        clients_[i]->Update(frametime);
    }
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
    configuration.setKeepAliveTimeout(15);
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

void XMPPModule::handleClientDisconnect(Client *client)
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
