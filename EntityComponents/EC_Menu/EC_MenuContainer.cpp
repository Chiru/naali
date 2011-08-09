/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_MenuContainer.cpp
 *  @brief  EC_MenuContainer creates 3D Menu in to scene.
 *  @note   no notes
 */


#include "StableHeaders.h"
#include "MenuDataModel.h"
#include "MenuDataItem.h"

#include "EC_MenuContainer.h"
#include "EC_MenuItem.h"
#include "EC_Mesh.h"
#include "EC_RigidBody.h"
#include "Entity.h"

#include <OgreCamera.h>
#include <OgreMath.h>
#include "RenderServiceInterface.h"

#include <QtGui>
#include <QStandardItemModel>

#include "InputAPI.h"
#include "AssetAPI.h"
#include "SceneManager.h"

#include "LoggingFunctions.h"
#include "MemoryLeakCheck.h"

#define SUBMENUCLOSE 1
#define SUBMENUCHANGE 0


DEFINE_POCO_LOGGING_FUNCTIONS("EC_MenuContainer")

EC_MenuContainer::EC_MenuContainer(IModule *module) :
    IComponent(module->GetFramework()),
    menudatamodel_(0),
    attachedMenuItem(0),
    menuClicked_(false),
    //subMenu_clicked_(false),
    subMenu_(false),
    //subMenuIsScrolling(false),
    startingPositionSaved_(false),
    menuIsRotating_(false),
    follow(this, "Follow camera", false),
    PhysicsEnabled(this, "Physics for menu", false),
    scrollerTimer_Interval(50),
    selected_(0),
    previousSelected_(0),
    subMenuItemSelected_(0),
    itemToRotate_(0),
    menulayer_(1),
    subMenuRadius_(0.0),
    radius_(0.0),
    rotationDirection_(0.0),
    item_offset_(0.0)
{
    scrollerTimer_ = new QTimer();
    rotatingTimer_ = new QTimer();

    // Connect signals from IComponent

    connect(this, SIGNAL(AttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)));
    QObject::connect(scrollerTimer_, SIGNAL(timeout()), this, SLOT(KineticScroller()));

    QObject::connect(rotatingTimer_, SIGNAL(timeout()), this, SLOT(RotatingMenu()));

    renderer_ = module->GetFramework()->GetService<Foundation::RenderServiceInterface>();

    //Create a new input context that menu will use to fetch input.
    input_ = module->GetFramework()->Input()->RegisterInputContext("MenuContainerInput", 100);

    // Grab the mouse movement input over Qt.
    input_->SetTakeMouseEventsOverQt(true);

    // Listen on mouse input signals.
    connect(input_.get(), SIGNAL(MouseEventReceived(MouseEvent *)), this, SLOT(HandleMouseInputEvent(MouseEvent *)));
}

EC_MenuContainer::~EC_MenuContainer()
{
    //if sceneManager is not found the client is allready running down.
    Scene::SceneManager *sceneManager_ = framework_->Scene()->GetDefaultScene().get();
    if(!sceneManager_)
    {
        MenuItemList_.clear();
    }

    //loop MenuItemList and subMenuItemList to clear all data from there.
    if(MenuItemList_.count()>0)
    {
        for(int i=0; i<MenuItemList_.count(); i++)
        {
            entity_id_t id = MenuItemList_.value(i)->GetParentEntity()->GetId();
            if(id)
                sceneManager_->RemoveEntity(id, AttributeChange::LocalOnly);
        }
        MenuItemList_.clear();
    }

    SAFE_DELETE(scrollerTimer_);
    SAFE_DELETE(rotatingTimer_);
    SAFE_DELETE(menudatamodel_);
}

void EC_MenuContainer::PrepareMenuContainer(float radius, MenuDataModel *parent)
{
    if(!parent)
        menudatamodel_ = new MenuDataModel();
    else
        menudatamodel_ = parent;

    radius_ = radius;
    //LogInfo("Menu radius_: "+ToString(radius_));
    SetMenuContainerPosition();
}

void EC_MenuContainer::ActivateMenu()
{
    //LogInfo("ActivateMenu()");
    for(int i=0; i<menudatamodel_->GetNumberOfDataItems();i++)
    {
        EC_MenuItem *menuItem = CreateMenuItem();
        menuItem->SetDataItem(menudatamodel_->GetMenuDataItem(i));
        MenuItemList_.append(menuItem);
    }

    ringmesh_ =  GetParentEntity()->GetOrCreateComponent("EC_Mesh", AttributeChange::LocalOnly, false);
    IComponent *iComponent = ringmesh_.get();
    if (iComponent)
    {
        //hardcoded test for menu visualization
        QString meshreference_ ="local://Torus_paa_vaaka.mesh";
        Vector3df position = Vector3df(0.0,0.8,0.0);
        EC_Mesh* mesh = dynamic_cast<EC_Mesh*>(iComponent);
        mesh->SetMeshRef(meshreference_);
        mesh->SetAdjustPosition(position);

        //mesh specific hack?
        Quaternion meshOrientation = mesh->GetAdjustOrientation();
        meshOrientation.z=90.0;
        meshOrientation.y=55.0;
        mesh->SetAdjustOrientation(meshOrientation);

        //calculate this scale somehow from radius(?)
        mesh->SetAdjustScale(Vector3df(1.5, 1.5, 1.5));
    }

    if(getPhysicsEnabled())
    {
        for(int i=0; i<MenuItemList_.count();i++)
        {
            EC_RigidBody *rigidbody = GetOrCreateRigidBody(MenuItemList_.at(i)->GetParentEntity());
            Vector3df zeroVec;
            zeroVec.set(0,0,0);
            rigidbody->setangularFactor(zeroVec); // Set zero angular factor so that body stays upright

        }
    }

    float phi;
    /// \todo add some logic to determine how large ring we need, based on number of items in it.

    for (int i = 0; i < MenuItemList_.count(); i++)
    {
        phi = 2 * float(i) * Ogre::Math::PI / float(MenuItemList_.count()) + ( 0.5*Ogre::Math::PI);
        EC_MenuItem *menuitem = MenuItemList_.at(i);
        //LogInfo(ToString(position));
        //LogInfo("Phi: " + ToString(phi));

        menuitem->setphi(phi);
        menuitem->SetMenuItemPosition(CalculateItemPosition(phi));
        menuitem->SetMenuItemVisible();
    }
}

void EC_MenuContainer::SetAttachedMenuItem(EC_MenuItem *attacheditem)
{
    //hardcoded radius, works with single ring-mesh
    radius_ = 2.0;
    attachedMenuItem = attacheditem;
    //Create new ring with given data.
    for(int i=0; i<attachedMenuItem->GetDataItem()->GetChildCount();i++)
    {
        EC_MenuItem *menuitem = CreateMenuItem();
        menuitem->SetDataItem(attachedMenuItem->GetDataItem()->GetChildDataItem(i));
        MenuItemList_.append(menuitem);
    }

    if(getPhysicsEnabled())
    {
        for(int i=0; i<MenuItemList_.count();i++)
        { 
            EC_RigidBody *rigidbody = GetOrCreateRigidBody(MenuItemList_.at(i)->GetParentEntity());
        }
    }

    //Counts which layer this is and based on that choose if menu is going to be horizontal or vertical.
    MenuDataItem* tempitem = attachedMenuItem->GetDataItem()->GetParentDataItem();
    menulayer_ += 1;
    while(tempitem)
    {
        tempitem = tempitem->GetParentDataItem();
        menulayer_++;
    }

    item_offset_=radius_ ;
    float phi;

    ringmesh_ = GetParentEntity()->CreateComponent("EC_Mesh", AttributeChange::LocalOnly, false);
    IComponent *iComponent = ringmesh_.get();
    if (iComponent)
    {
        //adds ring-mesh to scene.
        Vector3df position;
        Quaternion orientation = GetOrCreatePlaceableComponent()->GetOrientation();
        QString meshreference_;
        EC_Mesh *mesh = dynamic_cast<EC_Mesh*>(iComponent);

        switch(menulayer_)
        {
        //case 1 should newer occur here.
        case 2:
            meshreference_ ="local://Torus_pysty.mesh";
            position = Vector3df(0.0, -1.0, 0.0);
            mesh->SetAdjustPosition(position);
            mesh->SetAdjustOrientation(orientation.rotationFromTo(Vector3df(10.0, 20.0, 60.0),Vector3df(-50.0, 50.0, 180.0))); //magic numbers..
            break;

        case 3:
            meshreference_ ="local://Torus_ala_vaaka.mesh";
            position = Vector3df(radius_,-2.0,0.0);
            mesh->SetAdjustPosition(position);
            break;

        default:
            LogError("Invalid menulayer number!");
        }

        //Set newly created menuitems in some position..
        for(int i=0; i<MenuItemList_.count();i++)
        {
            phi = 2 * float(i) * Ogre::Math::PI / float(MenuItemList_.count()) + ( 0.5*Ogre::Math::PI);
            EC_MenuItem *menuitem = MenuItemList_.at(i);
            menuitem->setphi(phi);
            menuitem->SetMenuItemPosition(CalculateItemPosition(phi));
            menuitem->SetMenuItemVisible();
        }

        mesh->SetMeshRef(meshreference_);
    }
}

void EC_MenuContainer::ScaleMenu(float scale)
{

    Vector3df scalevector = Vector3df(1.0, 1.0, 1.0);
    scalevector *= scale;
    radius_ *= scale;

    //LogInfo(ToString(radius_));
    EC_Mesh *ring = dynamic_cast<EC_Mesh*>(ringmesh_.get());
    ring->SetAdjustScale(scalevector);

    for(int i=0; i<MenuItemList_.count(); i++)
    {
        //Scales items
        MenuItemList_.at(i)->SetScale(scalevector);
        //Move items to match scale.
        MenuItemList_.at(i)->SetMenuItemPosition(CalculateItemPosition(MenuItemList_.at(i)->getphi()));
    }
}

void EC_MenuContainer::SetMenuWidgets(QList<QWidget*> menuData)
{
    /// \todo refactor this initialization for submeshIndex and radius_
    int submeshIndex = 0;
    //radius_= 2.0;

    //MenuData_ = menuData;
    //numberOfMenuelements_ = menuData.count();
    //LogInfo("Components in menuData: " +ToString(numberOfMenuelements_));

    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    //Sets menu in front of camera
    SetMenuContainerPosition();

    float phi;
    //Set menuItem positions in circle.
    for(int i = 0; i < menuData.count(); i++)
    {
        EC_MenuItem *menuitem = CreateMenuItem();
        if(menuitem)
        {
            phi = 2 * float(i) * Ogre::Math::PI / float(menuData.count()) + ( 0.5*Ogre::Math::PI);
            menuitem->setphi(phi);
            menuitem->SetMenuItemPosition(CalculateItemPosition(phi));

            //hardcoded for now.. first item in every layout is "title" and rest of them are submenu items.
            /// \todo redesign this to support third data layer.
            //QLayoutItem * item = MenuData_.at(i)->layout()->itemAt(0);
            //assert(item);

            //isWidgetType() returns true if widget and it is much faster than dynamic_cast
            /*if(item->widget()->isWidgetType())
                menuItem->SetMenuItemWidget(submeshIndex, item->widget());
            else
                LogError("Failed to set data for menu item!");
            */
            //LogInfo("Items in submenu " + ToString(i) + ": " + ToString(MenuData_.at(i)->layout()->count()));
            MenuItemList_.append(menuitem);

        }
        else
            LogError("Error while creating menu items");
    }
}


void EC_MenuContainer::SetMenuContainerPosition()
{
    Scene::Entity *parent = GetParentEntity();
    Scene::EntityPtr avatarCameraPtr = parent->GetScene()->GetEntityByName("AvatarCamera");
    Scene::EntityPtr freeLookCameraPtr = parent->GetScene()->GetEntityByName("FreeLookCamera");

    //Offset for menu from camera coordinates
    /// \todo add setter function for these parameters
    distance.x=0;
    distance.y=3;
    distance.z=-18;


    if(avatarCameraPtr)
    {
        Scene::Entity *avatarCamera = avatarCameraPtr.get();
        cameraPlaceable = dynamic_cast<EC_Placeable*>(avatarCamera->GetComponent("EC_Placeable").get());
        //follow.Set(true, AttributeChange::LocalOnly);
        //LogInfo("avatarCamera is active");
    }
    else if(freeLookCameraPtr)
    {
        Scene::Entity *freeLookCamera = freeLookCameraPtr.get();
        cameraPlaceable = dynamic_cast<EC_Placeable*>(freeLookCamera->GetComponent("EC_Placeable").get());
        //LogInfo("freeLookCamera is active");
    }

    if(cameraPlaceable)
    {
        Transform cameraTransform = cameraPlaceable->gettransform();
        Vector3df cameraPosition = cameraTransform.position;

        distance = cameraPlaceable->GetRelativeVector(distance);

        ownEntityPos.x = cameraPosition.x+distance.x;
        ownEntityPos.y = cameraPosition.y+distance.y;
        ownEntityPos.z = cameraPosition.z+distance.z;

        Transform entityTransform;
        entityTransform.position=ownEntityPos;
        entityTransform.rotation=cameraTransform.rotation;

        //LogInfo(ToString(entityTransform.position));
        //LogInfo(ToString(cameraTransform.position));

        GetOrCreatePlaceableComponent()->settransform(entityTransform);
    }
    else
        LogError("Couldn't get OgreCamera Placeable");

}

void EC_MenuContainer::HandleMouseInputEvent(MouseEvent *mouse)
{
    //mouseinput
    if (mouse->IsLeftButtonDown() && !menuClicked_)
    {
        RaycastResult* result = 0;
        if(renderer_)
            result = renderer_->Raycast(mouse->X(), mouse->Y());
        assert(result);

        int i=0;
        while(!menuClicked_ && i < MenuItemList_.count())
        {
            if(result->entity_ == MenuItemList_.at(i)->GetParentEntity())
            {
                menuClicked_ = true;
                //to stop scrolling when clicked
                speed_ = 0;
            }
            i++;
        }
        if(!startingPositionSaved_)
        {
            startingPosition_.setX(mouse->X());
            startingPosition_.setY(mouse->Y());
            startingPositionSaved_ = true;
        }
    }

    if(menuClicked_ && mouse->IsLeftButtonDown() && mouse->eventType == MouseEvent::MouseMove)
    {
        for(int i = 0; i<MenuItemList_.count(); i++)
        {
            //Sets new angle for components using polar coordinates.
            float phi;
            if(menulayer_%2!=0)
                phi = MenuItemList_.at(i)->getphi() - (float)mouse->RelativeX()/250;
            else
                phi = MenuItemList_.at(i)->getphi() + (float)mouse->RelativeY()/250;

            MenuItemList_.at(i)->setphi(phi);
            //Next position for menu components.
            if(i != selected_)
                MenuItemList_.at(i)->SetMenuItemPosition(CalculateItemPosition(phi));
            else
            {
                //offset for selected item to popup from other items.
                MenuItemList_.at(i)->SetMenuItemPosition(CalculateItemPosition(phi, true));
            }


            if(Ogre::Math::Sin(phi) > 0.970)
            {
                previousSelected_ = selected_;
                selected_=i;
                if(SUBMENUCHANGE)
                {
                    if(previousSelected_ != selected_ && subMenu_)
                    {
                        CloseSubMenu(previousSelected_);
                        CreateSubMenu();
                    }
                }
                else
                {
                    if(previousSelected_ != selected_ && subMenu_)
                    {
                        CloseSubMenu(previousSelected_);
                    }
                }
            }
        }
        //LogInfo("Selected planar: " + ToString(selected_));
        if(menulayer_%2!=0)
            speed_=mouse->RelativeX();
        else
            speed_=-mouse->RelativeY();
    }

    if(mouse->eventType == MouseEvent::MouseReleased && menuClicked_)
    {
        if(speed_ == 0)
        {
            RaycastResult* result = 0;
            if(renderer_)
            result = renderer_->Raycast(mouse->X(), mouse->Y());
            assert(result);

            // SUBMENU HANDLER
            if(result->entity_ == MenuItemList_.at(selected_)->GetParentEntity() && !subMenu_)
            {
                if(!MenuItemList_.at(selected_)->OpenSubMenu())
                {
                    if(menulayer_==1)
                        emit OnMenuSelectionRaw(selected_, 0);
                    else
                        emit OnMenuSelection(selected_, 0, this);
                }
                else
                    CreateSubMenu();
            }
            else
            {
                if(subMenu_)
                    CloseSubMenu(selected_);

                int i=0;
                bool itemFound = false;
                while(!itemFound && i<MenuItemList_.count())
                {
                    if(result->entity_ == MenuItemList_.at(i)->GetParentEntity())
                        itemFound = true;
                    else
                        i++;
                }
                if(itemFound)
                {
                    switch(menulayer_)
                    {
                    case 1:
                        //horizontal, main layer
                        if(MenuItemList_.at(i)->GetMenuItemPosition().x>=0)
                            rotationDirection_=-0.1;
                        else if(MenuItemList_.at(i)->GetMenuItemPosition().x<=0)
                            rotationDirection_=0.1;
                        break;

                    case 2:
                        //vertical, sub layer
                        if(MenuItemList_.at(i)->GetMenuItemPosition().y>=0)
                            rotationDirection_=-0.1;
                        else if(MenuItemList_.at(i)->GetMenuItemPosition().y<=0)
                            rotationDirection_=0.1;

                        break;

                    case 3:
                        //horizontal, sub layer
                        if(MenuItemList_.at(i)->GetMenuItemPosition().x>=item_offset_)
                            rotationDirection_=-0.1;
                        else if(MenuItemList_.at(i)->GetMenuItemPosition().x<=item_offset_)
                            rotationDirection_=0.1;
                        break;
                    }

                    itemToRotate_ = i;
                    rotatingTimer_->setInterval(scrollerTimer_Interval);
                    rotatingTimer_->start();
                    menuIsRotating_ = true;
                    //RotateItemToSelected();

                    itemFound = false;
                }
            }
        }

        else if(speed_>3 || speed_<-3)
        {
            //LogInfo(ToString(speed_));
//            if(subMenu_clicked_)
//                subMenuIsScrolling = true;

            if(scrollerTimer_)
            {
                scrollerTimer_->setInterval(scrollerTimer_Interval);
                scrollerTimer_->start();
            }
        }
        else if(!menuIsRotating_)
        {
            RotateItemToSelected();
            if(scrollerTimer_)
                scrollerTimer_->stop();
            speed_=0;
        }
        menuClicked_ = false;
        //subMenu_clicked_ = false;
        startingPositionSaved_=false;
    }

}

void EC_MenuContainer::CloseSubMenu(int index)
{
    LogInfo("CloseSubMenu()");

    IComponent *component = 0;
    component = MenuItemList_.at(index)->GetParentEntity()->GetComponent("EC_MenuContainer").get();
    if(component)
    {
        EC_MenuContainer *childmenucontainer = dynamic_cast<EC_MenuContainer*>(component);

//        //debug
//        ComponentPtr ringptr = childmenucontainer->GetRingMesh();
//        assert(ringptr);
//        LogInfo("ringptr: "+ToString(ringptr));
//        LogInfo("childmenucontainer: "+ToString(childmenucontainer));
//        //debug ^^

        CloseSubMenu(childmenucontainer);
    }
    else
        LogError("Couldn't find child container!");
}

void EC_MenuContainer::CloseSubMenu(EC_MenuContainer *childmenucontainer)
{
//    LogInfo("CloseSubMenu() - childmenucontainer");

//    Vector3df position = Vector3df(-3.0, -3.0, 0.0);
//    position += GetOrCreatePlaceableComponent()->GetPosition();
//    GetOrCreatePlaceableComponent()->SetPosition(position);

    ComponentPtr ringptr = childmenucontainer->GetRingMesh();
    assert(ringptr);
    LogInfo(ToString(ringptr));

    MenuItemList_.at(selected_)->GetParentEntity()->RemoveComponent(ringptr);
    childmenucontainer->GetParentEntity()->RemoveComponent("EC_MenuContainer");
    subMenu_ = false;

}

void EC_MenuContainer::CreateSubMenu()
{

    if(MenuItemList_.at(selected_)->OpenSubMenu())
    {
//        Vector3df position = Vector3df(3.0, 3.0, 0.0);
//        position += GetOrCreatePlaceableComponent()->GetPosition();
//        GetOrCreatePlaceableComponent()->SetPosition(position);

        //Create new menucontainer to selected_ entity
        Scene::Entity *menuitementity = MenuItemList_.at(selected_)->GetParentEntity();
        IComponent *iComponent = menuitementity->GetOrCreateComponent("EC_MenuContainer", AttributeChange::LocalOnly, false).get();

        if(iComponent)
        {
            EC_MenuContainer *menucontainer = dynamic_cast<EC_MenuContainer*>(iComponent);
            menucontainer->SetAttachedMenuItem(MenuItemList_.at(selected_));

            //Connect signal from submenu
            connect(menucontainer, SIGNAL(OnMenuSelection(int, int, EC_MenuContainer*)), this, SLOT(ChildMenuClicked(int, int, EC_MenuContainer*)));
            subMenu_ = true;
        }

        else
        {
            LogError("Error while creating Menucontainer for submenu");
        }
    }
}

void EC_MenuContainer::ChildMenuClicked(int menuitem, int submenuItem, EC_MenuContainer* childcontainer)
{
    LogInfo("ChildMenuClicked: "+ToString(menuitem)+" "+ToString(submenuItem));
    CloseSubMenu(childcontainer);
}

Vector3df EC_MenuContainer::CalculateItemPosition(float phi, bool isSelected)
{
    //Vector3df selectedOffset = Vector3df(-0.5, 0.5, 0.5);
    Vector3df position = Vector3df(0.0, 0.0, 0.0);

    position.z = radius_ * sin(phi);

    switch(menulayer_)
    {
    case 1:
        //horizontal, main layer
        //LogInfo("horizontal, case 1");
        position.x = radius_ * cos(phi) + item_offset_;
        position.y = -0.5*position.z;
        break;

    case 2:
        //vertical
        //LogInfo("vertical, case 2");
        position.y = radius_ * cos(phi) - item_offset_;
        position.x = -0.5*position.z;
        position += Vector3df(0.0, 1.0, 0.0);
        break;

    case 3:
        //horizontal, sub layer
        //LogInfo("horizontal, case 3");
        position.x = radius_ * cos(phi) - item_offset_;
        position.y = -0.2*position.z;
        position += Vector3df(2*radius_, -2.0, 0.0);
        break;
    }

    if(isSelected)
    {
        //position+=selectedOffset;
        return position;
    }
    else
        return position;
}

void EC_MenuContainer::KineticScroller()
{
    if(menuIsRotating_)
    {
        menuIsRotating_ = false;
        rotatingTimer_->stop();
    }
    if(speed_!=0 /*&& !menuIsRotating_*/)
    {
        for(int i=0; i<MenuItemList_.count(); i++)
        {
            float phi = MenuItemList_.at(i)->getphi() - speed_ * scrollerTimer_Interval/10000;
            MenuItemList_.at(i)->setphi(phi);
            if(i!=selected_)
                MenuItemList_.at(i)->SetMenuItemPosition(CalculateItemPosition(phi));
            else
                MenuItemList_.at(i)->SetMenuItemPosition(CalculateItemPosition(phi, true));

            if(Ogre::Math::Sin(phi) > 0.970)
            {
                previousSelected_ = selected_;
                selected_=i;

                if(SUBMENUCHANGE)
                {
                    if(subMenu_ && previousSelected_ != selected_)
                    {
                        CloseSubMenu(previousSelected_);
                        CreateSubMenu();
                    }
                }
                else
                {
                    if(previousSelected_ != selected_ && subMenu_)
                    {
                        CloseSubMenu(previousSelected_);
                        CreateSubMenu();
                    }
                }
            }
        }

        if(speed_<0)
            speed_ += 1.0;
        else
            speed_ -= 1.0;

        if(speed_ > -1 && speed_ < 1)
            speed_=0;


//        if(speed_ == 0)
//            subMenuIsScrolling = false;

        //LogInfo("speed: " + ToString(speed_));
    }

    else
    {
        RotateItemToSelected();
        scrollerTimer_->stop();
    }
}

void EC_MenuContainer::RotatingMenu()
{
    if(menuIsRotating_)
    {
        for(int i=0; i<MenuItemList_.count(); i++)
        {
            float phi = MenuItemList_.at(i)->getphi() - rotationDirection_;
            MenuItemList_.at(i)->setphi(phi);
            //LogInfo("i: "+ToString(i)+" phi: "+ToString(phi)+" itemphi: "+ToString(MenuItemList_.at(i)->getphi()));

            if(Ogre::Math::Sin(phi) > 0.970)
            {
                MenuItemList_.at(i)->SetMenuItemPosition(CalculateItemPosition(phi,true));
                previousSelected_ = selected_;
                selected_ = i;
            }
            else
                MenuItemList_.at(i)->SetMenuItemPosition(CalculateItemPosition(phi));

            if(selected_ == itemToRotate_)
            {
                menuIsRotating_ = false;
                rotatingTimer_->stop();
                RotateItemToSelected();
            }
        }
    }
    else
        RotateItemToSelected();
}

void EC_MenuContainer::RotateItemToSelected()
{
    //Rotate scroller so, that selected component is in the front.
    //Original positions. We want that planars are in those positions after scrolling is over.
    /*
        position.x=2 * cos( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
        position.z=2 * sin( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
    */

    float phi = 0.5*(float)Ogre::Math::PI;
    if(MenuItemList_.count()>0)
    {
        /// \todo Add functionality to change the selected menuitem if mouse is moved more than 10 in x-axis after clicking.

        MenuItemList_.at(selected_)->setphi(phi);
        MenuItemList_.at(selected_)->SetMenuItemPosition(CalculateItemPosition(phi, true));

        //LogInfo("Selected planar: " + ToString(selected_));
        int j = selected_;
        for (int i=1; i<MenuItemList_.count(); i++)
        {
            j++;
            if(j==MenuItemList_.count())
                j=0;

            phi = 2 * float(i) * Ogre::Math::PI / float(MenuItemList_.count()) + ( 0.5*Ogre::Math::PI);
            MenuItemList_.at(j)->setphi(phi);
            MenuItemList_.at(j)->SetMenuItemPosition(CalculateItemPosition(phi));
        }
    }
}

EC_MenuItem* EC_MenuContainer::CreateMenuItem()
{
    ComponentPtr parent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false);
    return CreateMenuItem(parent);
}

EC_MenuItem* EC_MenuContainer::CreateMenuItem(ComponentPtr parentPlaceable)
{
    Scene::SceneManager *sceneManager_ = framework_->Scene()->GetDefaultScene().get();

    entity_id_t id = sceneManager_->GetNextFreeIdLocal();
    Scene::EntityPtr entity_ = sceneManager_->CreateEntity(id, QStringList(), AttributeChange::LocalOnly, false);

    //LogInfo("Pointer " + ToString(entity_));
    if(!entity_)
        LogError("Couldn't create entity with given ID");
    else
    {
        Scene::Entity *MenuItemEntity = entity_.get();
        IComponent *iComponent = MenuItemEntity->GetOrCreateComponent("EC_MenuItem", AttributeChange::LocalOnly, false).get();

        if(iComponent)
        {
            EC_MenuItem *menuItem = dynamic_cast<EC_MenuItem*>(iComponent);
            //Sets parent entity for menuItem-entitys placeable component
            menuItem->SetParentMenuContainer(parentPlaceable);
            sceneManager_->EmitEntityCreated(MenuItemEntity, AttributeChange::LocalOnly);
            return menuItem;
        }
        else
        {
            LogError("Error while creating MenuItem");
        }
    }
    return 0;
}

QObject* EC_MenuContainer::GetMenuDataModel()
{
    //LogInfo("GetMenuDataModel - before");
    if(menudatamodel_)
    {
        //LogInfo("GetMenuDataModel - if" +ToString(menudatamodel_));
        return menudatamodel_;
    }
    else
    {
        //LogInfo("GetMenuDataModel - else" +ToString(menudatamodel_));
        menudatamodel_ = new MenuDataModel();
        return menudatamodel_;
    }
}

EC_RigidBody* EC_MenuContainer::GetOrCreateRigidBody(Scene::Entity* entity)
{
    IComponent *iComponent = entity->GetOrCreateComponent("EC_RigidBody", AttributeChange::LocalOnly, false).get();
    EC_RigidBody *rigidbody = dynamic_cast<EC_RigidBody*>(iComponent);
    rigidbody->setmass(1.0);
    rigidbody->setgravityEnabled(true);
    rigidbody->RespectMyAuthority(true);
    //sceneManager_->EmitEntityCreated(MenuItemEntity, AttributeChange::LocalOnly);

    return rigidbody;
}

EC_Placeable *EC_MenuContainer::GetOrCreatePlaceableComponent()
{
    if (!GetParentEntity())
        return 0;
    //IComponent *iComponent = parent->GetOrCreateComponentRaw(EC_3DCanvas::TypeNameStatic(), AttributeChange::LocalOnly, false);
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    return placeable;
}

void EC_MenuContainer::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{

}

void EC_MenuContainer::AttributeChanged(IAttribute* attribute, AttributeChange::Type change)
{
    if(attribute == &PhysicsEnabled)
    {
        if(PhysicsEnabled.Get() == true)
        {
            for(int i=0;i<MenuItemList_.count();i++)
            {
                GetOrCreateRigidBody(MenuItemList_.at(i)->GetParentEntity());
            }
        }
        else
        {
            for(int i=0;i<MenuItemList_.count();i++)
            {
                MenuItemList_.at(i)->GetParentEntity()->RemoveComponent("EC_RigidBody");
            }
        }
    }

    if (attribute == &follow)
    {
        Scene::Entity *parent = GetParentEntity();
        Scene::EntityPtr avatarCameraPtr = parent->GetScene()->GetEntityByName("AvatarCamera");
        Scene::EntityPtr freeLookCameraPtr = parent->GetScene()->GetEntityByName("FreeLookCamera");

        if(follow.Get() == true)
        {
            if(avatarCameraPtr)
            {
                Scene::Entity *avatarCamera = avatarCameraPtr.get();
                GetOrCreatePlaceableComponent()->SetParent(avatarCamera->GetComponent("EC_Placeable"));
                //LogInfo("follow avatarCamera is active");
            }
            else if(freeLookCameraPtr)
            {
                Scene::Entity *freeLookCamera = freeLookCameraPtr.get();
                GetOrCreatePlaceableComponent()->SetParent(freeLookCamera->GetComponent("EC_Placeable"));
                //LogInfo("follow freeLookCamera is active");
            }
            distance.x=0;
            distance.y=-2;
            distance.z=-25;

            Transform entityTransform;
            entityTransform.position=distance;

            GetOrCreatePlaceableComponent()->settransform(entityTransform);

        }
        else
        {
            GetParentEntity()->RemoveComponent(GetParentEntity()->GetComponent("EC_Placeable"));
            ComponentPtr parentPlaceable = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false);

            distance.z=-25;
            distance.y=-2;

            if(cameraPlaceable)
            {
                Transform cameraTransform = cameraPlaceable->gettransform();
                Vector3df cameraPosition = cameraTransform.position;

                distance = cameraPlaceable->GetRelativeVector(distance);

                ownEntityPos.x = cameraPosition.x+distance.x;
                ownEntityPos.y = cameraPosition.y+distance.y;
                ownEntityPos.z = cameraPosition.z+distance.z;

                Transform entityTransform;
                entityTransform.position=ownEntityPos;
                entityTransform.rotation=cameraTransform.rotation;

                //LogInfo(ToString(entityTransform.position));
                //LogInfo(ToString(cameraTransform.position));

                GetOrCreatePlaceableComponent()->settransform(entityTransform);
                for(int i=0; i<MenuItemList_.count();i++)
                {
                    MenuItemList_.at(i)->SetParentMenuContainer(parentPlaceable);
                }
            }
            else
                LogError("Couldn't get OgreCamera Placeable");
        }
    }
}
