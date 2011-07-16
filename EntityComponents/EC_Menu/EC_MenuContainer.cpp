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

#define SUBMENUCLOSE 0
#define SUBMENUCHANGE 1


DEFINE_POCO_LOGGING_FUNCTIONS("EC_MenuContainer")

EC_MenuContainer::EC_MenuContainer(IModule *module) :
    IComponent(module->GetFramework()),
    menudatamodel_(0),
    attachedMenuItem(0),
    menuClicked_(false),
    subMenu_clicked_(false),
    subMenu_(false),
    subMenuIsScrolling(false),
    startingPositionSaved_(false),
    follow(this, "Follow camera", false),
    selected_(0),
    previousSelected_(0),
    subMenuItemSelected_(0),
    menulayer_(1),
    subMenuRadius_(0.0),
    radius_(0.0)
{
    scrollerTimer_ = new QTimer();

    // Connect signals from IComponent
    //connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareMenuContainer()), Qt::UniqueConnection);
    //connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
    connect(this, SIGNAL(AttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)));
    QObject::connect(scrollerTimer_, SIGNAL(timeout()), this, SLOT(KineticScroller()));

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
    SAFE_DELETE(menudatamodel_);
}

void EC_MenuContainer::PrepareMenuContainer(float radius, MenuDataModel *parent)
{
    if(!parent)
    {
        //LogInfo("PrepareMenuContainer() - if");
        menudatamodel_ = new MenuDataModel();
    }
    else
    {
        //LogInfo("PrepareMenuContainer() - else");
        menudatamodel_ = parent;
    }

    radius_ = radius;
    //LogInfo("Menu radius_: "+ToString(radius_));
    SetMenuContainerPosition();
}

void EC_MenuContainer::AddComponentToMenu(EC_MenuItem *menuitem)
{
    MenuItemList_.append(menuitem);
}

void EC_MenuContainer::AddComponentToMenu(QString meshref, QStringList materials, int itemnumber)
{
    if(itemnumber)
    {
        if(!menudatamodel_->AddItemToIndex(meshref, materials, itemnumber))
            LogError("Error while adding menuitemdata to container! (error 1)");
    }
    else
    {
        if(!menudatamodel_->AddItem(meshref, materials))
            LogError("Error while adding menuitemdata to container! (error 2)");
    }

//    EC_MenuItem *menuItem = CreateMenuItem();
//    menuItem->SetMenuItemMesh(meshref, materials);
//    assert(menuItem);
//    if(itemnumber==0)
//        MenuItemList_.append(menuItem);
//    else
//        MenuItemList_.at(itemnumber)->AddComponentToSubMenu(menuItem);

}

void EC_MenuContainer::ActivateMenu()
{
    LogInfo("ActivateMenu()");
    for(int i=0; i<menudatamodel_->GetNumberOfDataItems();i++)
    {
        EC_MenuItem *menuItem = CreateMenuItem();
        menuItem->SetDataItem(menudatamodel_->GetMenuDataItem(i));
        MenuItemList_.append(menuItem);
    }

    Vector3df position = Vector3df(0.0, 0.0, 0.0);
    float phi;
    for (int i = 0; i < MenuItemList_.count(); i++)
    {
        phi = 2 * float(i) * Ogre::Math::PI / float(MenuItemList_.count()) + ( 0.5*Ogre::Math::PI);
        position.x = radius_ * cos(phi);
        position.z = radius_ * sin(phi);

        EC_MenuItem *menuitem = MenuItemList_.at(i);
        //LogInfo(ToString(position));
        //LogInfo("Phi: " + ToString(phi));
        menuitem->setphi(phi);
        menuitem->SetMenuItemPosition(position);
        menuitem->SetMenuItemVisible();

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

    Vector3df position = Vector3df(0.0, 0.0, 0.0);
    float phi;

    //Set menuItem positions in circle.
    for(int i = 0; i < menuData.count(); i++)
    {
        EC_MenuItem *menuItem = CreateMenuItem();
        if(menuItem)
        {
            phi = 2 * float(i) * Ogre::Math::PI / float(menuData.count()) + ( 0.5*Ogre::Math::PI);
            position.x = radius_ * cos(phi);
            position.z = radius_ * sin(phi);

            //LogInfo(ToString(position));
            //LogInfo(ToString(phi));
            menuItem->setphi(phi);
            menuItem->SetMenuItemPosition(position);


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
            MenuItemList_.append(menuItem);

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
    distance.z=-12;
    distance.y=-2;

    if(avatarCameraPtr)
    {
        Scene::Entity *avatarCamera = avatarCameraPtr.get();
        cameraPlaceable = dynamic_cast<EC_Placeable*>(avatarCamera->GetComponent("EC_Placeable").get());
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
    /// \todo refactor mouseinput to handle different menulevels. ATM supports only 2 menu layers.
    //mouseinput

    if(mouse->IsLeftButtonDown() && !startingPositionSaved_)
    {
        mousePosition.setX(mouse->X());
        mousePosition.setY(mouse->Y());
        startingPositionSaved_=true;
    }

    if (mouse->IsLeftButtonDown() && !menuClicked_)
    {
        RaycastResult* result = 0;
        int i=0;
        if(renderer_)
            result = renderer_->Raycast(mouse->X(), mouse->Y());

        assert(result);
        while(!menuClicked_ && !subMenu_clicked_ && i<MenuItemList_.count())
        {
            if(result->entity_ == MenuItemList_.at(i)->GetParentEntity())
            {
                menuClicked_ = true;
                //to stop scrolling when clicked
                speed_ = 0;
            }
            i++;
        }
        if(!menuClicked_)
        {/*
            int i=0;
            while(!subMenu_clicked_ && i<subMenuItemList_.count())
            {
                if(result->entity_ == subMenuItemList_.at(i)->GetParentEntity())
                {
                    subMenu_clicked_ = true;
                    //to stop scrolling when clicked
                    speed_ = 0;
                }
                i++;
            }*/
        }
    }

    if(subMenu_clicked_ && mouse->IsLeftButtonDown() && mouse->eventType == MouseEvent::MouseMove)
    {
        speed_= -(mouse->RelativeY());
    }

    if(menuClicked_ && mouse->IsLeftButtonDown() && mouse->eventType == MouseEvent::MouseMove)
    {

        if(SUBMENUCLOSE)
        {
            if(subMenu_)
            {
                Scene::SceneManager *sceneManager_ = framework_->Scene()->GetDefaultScene().get();
                assert(sceneManager_);
            }
        }

        Vector3df position = Vector3df(0.0,0.0,0.0);
        for(int i = 0; i<MenuItemList_.count(); i++)
        {
            //Sets new angle for components using polar coordinates.
            float phi = MenuItemList_.at(i)->getphi() - float((float)mouse->RelativeX()/250);

            //Next position for menu components.

            if(menulayer_%2!=0)
                position.x = radius_ * cos(phi);
            else
                position.y = radius_ * cos(phi);
            position.z = radius_ * Ogre::Math::Sin(phi);
            MenuItemList_.at(i)->setphi(phi);

            if(Ogre::Math::Sin(phi) > 0.950)
            {
                previousSelected_ = selected_;
                selected_=i;

                //Predefined usecase.
                if(SUBMENUCHANGE)
                {
                    if(subMenu_ && previousSelected_ != selected_)
                    {
                        Scene::SceneManager *sceneManager_ = framework_->Scene()->GetDefaultScene().get();
                        assert(sceneManager_);

//                        MenuItemList_.at(selected_)->OpenSubMenu();

                    }
                }
            }

            MenuItemList_.at(i)->SetMenuItemPosition(position);
        }
        //LogInfo("Selected planar: " + ToString(selected_));
        speed_=mouse->RelativeX();
    }

    if(mouse->eventType == MouseEvent::MouseReleased && (menuClicked_ || subMenu_clicked_))
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
                //returns false if item don't have childs
                if(!MenuItemList_.at(selected_)->OpenSubMenu())
                {
                    emit OnMenuSelection(selected_, 0);
                }
                else
                {
                    //Create new menucontainer to selected_ entity
                    Scene::Entity *menuitementity = MenuItemList_.at(selected_)->GetParentEntity();
                    IComponent *iComponent = menuitementity->GetOrCreateComponent("EC_MenuContainer", AttributeChange::LocalOnly, false).get();

                    if(iComponent)
                    {
                        EC_MenuContainer *menucontainer = dynamic_cast<EC_MenuContainer*>(iComponent);
                        menucontainer->SetAttachedMenuItem(MenuItemList_.at(selected_));

                    }
                    else
                        LogError("Error while creating Menucontainer for submenu");

                }

            }
        }

        if(speed_>3 || speed_<-3)
        {
            scrollerTimer_Interval=50;
            //LogInfo(ToString(speed_));
            if(subMenu_clicked_)
                subMenuIsScrolling = true;

            if(scrollerTimer_)
            {
                scrollerTimer_->setInterval(scrollerTimer_Interval);
                scrollerTimer_->start();
            }
        }
        else
        {
            CenterAfterRotation();
            if(scrollerTimer_)
                scrollerTimer_->stop();
            speed_=0;
        }
        menuClicked_ = false;
        subMenu_clicked_ = false;
        startingPositionSaved_=false;
    }

}

//***************************************************
void EC_MenuContainer::SetAttachedMenuItem(EC_MenuItem *attacheditem)
{
    radius_ = 3.0;
    attachedMenuItem = attacheditem;
    for(int i=0; i<attachedMenuItem->GetDataItem()->GetChildCount();i++)
    {
        LogInfo("Creating submenu ring.. " + ToString(i));
        //Create new ring with given data.

        EC_MenuItem *menuitem = CreateMenuItem();
        menuitem->SetDataItem(attachedMenuItem->GetDataItem()->GetChildDataItem(i));
        MenuItemList_.append(menuitem);
    }

    //Counts which layer this is and based on that choose if menu is going to be horizontal or vertical.
    MenuDataItem* tempitem = attachedMenuItem->GetDataItem()->GetParentDataItem();
    menulayer_ = 0;
    while(tempitem)
    {
        tempitem = tempitem->GetParentDataItem();
        menulayer_++;
    }

    Vector3df position = Vector3df(0.0, 0.0, 0.0);
    float phi;
    for(int i=0; i<MenuItemList_.count();i++)
    {
        //Set newly created menuitems in some position..

        phi = 2 * float(i) * Ogre::Math::PI / float(MenuItemList_.count()) + ( 0.5*Ogre::Math::PI);
        if(menulayer_%2!=0)
            position.x = radius_ * cos(phi);
        else
            position.y = radius_ * cos(phi);

        position.z = radius_ * sin(phi);

        EC_MenuItem *menuitem = MenuItemList_.at(i);
        //LogInfo(ToString(position));
        //LogInfo("Phi: " + ToString(phi));
        menuitem->setphi(phi);
        menuitem->SetMenuItemPosition(position);
        menuitem->SetMenuItemVisible();
    }
}

void EC_MenuContainer::KineticScroller()
{
    if(speed_!=0)
    {
        Vector3df position = Vector3df(0.0,0.0,0.0);

        for(int i=0; i<MenuItemList_.count(); i++)
        {
            float phi = MenuItemList_.at(i)->getphi() - speed_ * scrollerTimer_Interval/10000;
            if(menulayer_%2!=0)
                position.x = radius_ * cos(phi);
            else
                position.y = radius_ * cos(phi);
            position.z = radius_ * sin(phi);

            MenuItemList_.at(i)->setphi(phi);
            MenuItemList_.at(i)->SetMenuItemPosition(position);

            if(Ogre::Math::Sin(phi) > 0.950)
            {
                previousSelected_ = selected_;
                selected_=i;
                if(SUBMENUCHANGE)
                {
                    /// \todo This functionality need some optimization. Also some logic upgrade could be needed.
                    if(subMenu_ && previousSelected_ != selected_)
                    {
                        //Call for selectec menuitem to close it's submenu or emit signal to do that?
                    }
                }
            }
        }
            //LogInfo("Selected planar: " + ToString(selected_));

        if(speed_<0)
            speed_ += 1.0;
        else
            speed_ -= 1.0;

        if(speed_ == 0)
            subMenuIsScrolling = false;

        //LogInfo("speed: " + ToString(speed_));
    }

    else
    {
        CenterAfterRotation();
        scrollerTimer_->stop();
    }
}

void EC_MenuContainer::CenterAfterRotation()
{
    //Rotate scroller so, that selected component is in the front.
    //Original positions. We want that planars are in those positions after scrolling is over.
    /*
        position.x=2 * cos( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
        position.z=2 * sin( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
    */

    Vector3df position = Vector3df(0.0,0.0,0.0);

    float phi;
    phi = 0.5*(float)Ogre::Math::PI;

    if(MenuItemList_.count()>0)
    {
        /// \todo Add functionality to change the selected menuitem if mouse is moved more than 10 in x-axis after clicking.

        if(menulayer_%2!=0)
            position.x = radius_ * cos(phi);
        else
            position.y = radius_ * cos(phi);
        position.z = radius_ * sin( phi );

        MenuItemList_.at(selected_)->SetMenuItemPosition(position);
        MenuItemList_.at(selected_)->setphi(phi);

        //LogInfo("Selected planar: " + ToString(selected_));
        int j = selected_;
        for (int i=1; i<MenuItemList_.count(); i++)
        {
            j++;

            if(j==MenuItemList_.count())
                j=0;

            phi = 2 * float(i) * Ogre::Math::PI / float(MenuItemList_.count()) + ( 0.5*Ogre::Math::PI);
            if(menulayer_%2!=0)
                position.x = radius_ * cos(phi);
            else
                position.y = radius_ * cos(phi);
            position.z = radius_ * sin(phi);

            MenuItemList_.at(j)->setphi(phi);
            MenuItemList_.at(j)->SetMenuItemPosition(position);
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
            LogError("Error while creating MenuItem");
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
            distance.z=-12;

            Transform entityTransform;
            entityTransform.position=distance;

            GetOrCreatePlaceableComponent()->settransform(entityTransform);

        }
        else
        {
            GetParentEntity()->RemoveComponent(GetParentEntity()->GetComponent("EC_Placeable"));
            ComponentPtr parentPlaceable = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false);

            distance.z=-12;
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
