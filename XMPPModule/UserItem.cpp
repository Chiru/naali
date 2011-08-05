// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "UserItem.h"
#include "XMPPModule.h"

#include "CoreStringUtils.h"

#include "MemoryLeakCheck.h"

namespace XMPP
{
    UserItem::UserItem(const QXmppRosterIq::Item &rosterItem) :
        has_vcard_(false),
        available_(false)
    {
        bare_jid_ = rosterItem.bareJid();
    }

    UserItem::~UserItem()
    {

    }

    void UserItem::checkAvailability()
    {
        bool available = false;

        if(resources_.size() == 0)
        {
            available_ = false;
            emit availabilityChanged(available_);
            return;
        }

        QStringList resources = resources_.keys();
        QString resource;
        foreach(resource, resources)
        {
            if(resources_[resource].available)
                available = true;
        }

        if(available_ != available)
        {
            available_ = available;
            emit availabilityChanged(available_);

            if(available_)
                XMPPModule::LogDebug(bare_jid_.toStdString() + " changed availability: true");
            else
                XMPPModule::LogDebug(bare_jid_.toStdString() + " changed availability: false");
        }
    }

    void UserItem::updatePresence(const QString &resource, const QXmppPresence &presence)
    {
        if(presence.type() == QXmppPresence::Available)
        {
            if(resources_.contains(resource))
                resources_.remove(resource);

            ResourceItem resource_item;
            resource_item.available = true;

            QStringList capabilities = presence.capabilityExt();
            QString capability;
            foreach(capability, capabilities)
                resource_item.capabilities.append(capability);

            resources_.insert(resource, resource_item);
            checkAvailability();
        }
        else if(presence.type() == QXmppPresence::Unavailable)
        {
            if(resources_.contains(resource))
                resources_.remove(resource);
            checkAvailability();
        }
    }

    void UserItem::updateRosterItem(const QXmppRosterIq::Item &item)
    {
        /// \note do we really need this at all?
    }

    void UserItem::updateVCard(const QXmppVCardIq &vcard)
    {
        birthday_ = vcard.birthday().toString("dd.MM.yyyy");
        email_ = vcard.email();
        full_name_ = vcard.fullName();
        photo_ = vcard.photo();
        url_ = vcard.url();
    }

    QStringList UserItem::getCapabilities()
    {
        QStringList capabilities;

        QString resource;
        foreach(resource, resources_.keys())
        {
            QString capability;
            foreach(capability, resources_[resource].capabilities)
            {
                if(!capabilities.contains(capability))
                    capabilities.append(capability);
            }
        }

        return capabilities;
    }
} // end of namespace: XMPP

