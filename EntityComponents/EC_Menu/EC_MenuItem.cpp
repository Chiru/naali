/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_MenuItem.cpp
 *  @brief  EC_MenuItem creates single element of 3D Menu component in to scene.
 *  @note   no notes
 */

#include "StableHeaders.h"

#include "EC_MenuItem.h"
#include "EC_MenuContainer.h"

#include "EC_3DCanvas.h"
#include "EC_Mesh.h"
#include "EC_Placeable.h"
#include "Entity.h"
//#include "AssetReference.h"

#include "LoggingFunctions.h"
#include "MemoryLeakCheck.h"

DEFINE_POCO_LOGGING_FUNCTIONS("EC_MenuItem")

EC_MenuItem::EC_MenuItem(IModule *module) :
    IComponent(module->GetFramework()),
    phi(this, "Phi", 0.0),
    meshreference_(""),
    widget_(0),
    widgetSubmesh_(0)
{
    // Connect signals from IComponent
    connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareMenuItem()), Qt::UniqueConnection);
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
}

EC_MenuItem::~EC_MenuItem()
{
}

void EC_MenuItem::PrepareMenuItem()
{    
    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    // Get parent and connect to the signals.
    Scene::Entity *parent = GetParentEntity();
    assert(parent);
    if (parent)
    {
        connect(parent, SIGNAL(ComponentRemoved(IComponent*, AttributeChange::Type)), SLOT(ComponentRemoved(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
    }
    else
    {
        LogError("PrepareComponent: Could not get parent entity pointer!");
        return;
    }

}

void EC_MenuItem::SetDataItem(MenuDataItem *dataitemptr)
{
    itemdata_ = dataitemptr;
    SetMenuItemMesh(itemdata_->GetMeshRef(), itemdata_->GetMaterialRef());
}

MenuDataItem* EC_MenuItem::GetDataItem()
{
    return itemdata_;
}

void EC_MenuItem::SetMenuItemMesh(QString meshref, QStringList materials)
{
    meshreference_=meshref;
    for(int i=0; i<materials.count();i++)
    {
        materials_.Append(AssetReference(materials.at(i)));
    }
}

void EC_MenuItem::SetMenuItemPosition(Vector3df position)
{
    GetOrCreatePlaceableComponent()->SetPosition(position);
}

void EC_MenuItem::SetMenuItemWidget(int subMeshIndex, QWidget *data)
{
    widget_ = data;
    widgetSubmesh_ = subMeshIndex;
}

void EC_MenuItem::SetParentMenuContainer(ComponentPtr MenuContainer)
{
    //setter-function for setting entity position.
    EC_Placeable *placeable = GetOrCreatePlaceableComponent();
    if(placeable && MenuContainer)
        placeable->SetParent(MenuContainer);
}

void EC_MenuItem::SetMenuItemVisible()
{
    if(!meshreference_.isEmpty())
    {
        EC_Mesh *mesh = GetOrCreateMeshComponent();
        mesh->SetMeshRef(meshreference_);
        if(materials_.Size()>0)
        {
            AttributeChange::Type type = AttributeChange::Default;
            mesh->meshMaterial.Set(materials_, type);
        }
    }

    if(widget_)
    {
        EC_3DCanvas *canvas = GetOrCreateCanvasComponent();
        canvas->SetSubmesh(widgetSubmesh_);
        canvas->SetWidget(widget_);
    }
}

EC_3DCanvas* EC_MenuItem::GetOrCreateCanvasComponent()
{
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_3DCanvas", AttributeChange::LocalOnly, false).get();
    if (iComponent)
    {
        EC_3DCanvas *canvas = dynamic_cast<EC_3DCanvas*>(iComponent);
        return canvas;
    }
    else
    {
        LogError("Couldn't get or greate EC_3DCanvas");
        return 0;
    }
}

EC_Mesh* EC_MenuItem::GetOrCreateMeshComponent()
{
    IComponent *iComponent =  GetParentEntity()->GetOrCreateComponent("EC_Mesh", AttributeChange::LocalOnly, false).get();
    if (iComponent)
    {
        EC_Mesh *mesh = dynamic_cast<EC_Mesh*>(iComponent);
        return mesh;
    }
    else
    {
        LogError("Couldn't get or greate EC_Mesh, returning null pointer");
        return 0;
    }
}

EC_Placeable *EC_MenuItem::GetOrCreatePlaceableComponent()
{
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    if(iComponent)
    {
        EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
        return placeable;
    }
    else
    {
        LogError("Couldn't get or create EC_Placeable");
        return 0;
    }
}

void EC_MenuItem::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{
}

void EC_MenuItem::ComponentAdded(IComponent *component, AttributeChange::Type change)
{
}

void EC_MenuItem::AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType)
{
}
