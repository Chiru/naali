/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Menu.cpp
 *  @brief  EC_Menu creates 3D Menu component in to scene.
 *  @note   no notes
 */


#include "StableHeaders.h"
#include "EC_MenuContainer.h"
#include "EC_MenuItem.h"
#include "IModule.h"
#include "Entity.h"
#include "LoggingFunctions.h"

#include <Ogre.h>
#include <EC_OgreCamera.h>
#include "RenderServiceInterface.h"

#include <QStringListModel>
#include <QListView>
//#include <QMouseEvent>
//#include "MouseEvent.h"
#include "InputAPI.h"

#include "SceneInteract.h"

#include "SceneManager.h"
#include "UiAPI.h"
#include "SceneAPI.h"

#include <QLabel>

#include "MemoryLeakCheck.h"

DEFINE_POCO_LOGGING_FUNCTIONS("EC_MenuContainer")

EC_MenuContainer::EC_MenuContainer(IModule *module) :
    IComponent(module->GetFramework()),
    ent_clicked_(false),
    subMenu_clicked_(false),
    subMenu_(false),
    subMenuIsScrolling(false),
    numberOfMenuelements_(0),
    selected_(0),
    subMenuRadius_(0.0),
    radius_(0.0)
{
    scrollerTimer_ = new QTimer();
    renderTimer_ = new QTimer();

    // Connect signals from IComponent
    //connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareMenuContainer()), Qt::UniqueConnection);
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
    QObject::connect(scrollerTimer_, SIGNAL(timeout()), this, SLOT(kinecticScroller()));
    QObject::connect(renderTimer_, SIGNAL(timeout()), this, SLOT(Render()));
    renderTimer_->setInterval(40);
    renderTimer_->start();

    renderer_ = module->GetFramework()->GetService<Foundation::RenderServiceInterface>();

    //Create a new input context that menu will use to fetch input.
    input_ = module->GetFramework()->Input()->RegisterInputContext("MenuContainerInput", 100);

    // To be sure that Qt doesn't play tricks on us and miss a mouse release when we're in FPS mode,
    // grab the mouse movement input over Qt.
    input_->SetTakeMouseEventsOverQt(true);

    // Listen on mouse input signals.
    connect(input_.get(), SIGNAL(MouseEventReceived(MouseEvent *)), this, SLOT(HandleMouseInputEvent(MouseEvent *)));
}

EC_MenuContainer::~EC_MenuContainer()
{
    //loop MenuItemList and delete every entity from there.
    if(MenuItemList_.count()>0)
    {
        Scene::SceneManager *sceneManager_ = framework_->Scene()->GetDefaultScene().get();
        for(int i=0; i<MenuItemList_.count(); i++)
        {
            entity_id_t id = MenuItemList_.at(i)->GetParentEntity()->GetId();
            if(id)
                sceneManager_->RemoveEntity(id, AttributeChange::LocalOnly);
        }
        MenuItemList_.clear();
        for(int i=0; i<SubMenuItemList_.count(); i++)
        {
            entity_id_t id = SubMenuItemList_.at(i)->GetParentEntity()->GetId();
            if(id)
                sceneManager_->RemoveEntity(id, AttributeChange::LocalOnly);
        }
        SubMenuItemList_.clear();
    }
    SAFE_DELETE(scrollerTimer_);
    SAFE_DELETE(renderTimer_);
}

void EC_MenuContainer::SetMenuData(int menuElements, float radius)
{
    numberOfMenuelements_ = menuElements;
    radius_=radius;
    PrepareMenuContainer();
    //MenuData=menuData;
}

void EC_MenuContainer::Render()
{
    /// \todo This needs some serious optimization
    //LogInfo("Trying to render");

    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    for(int i=0; i<SubMenuItemList_.count(); i++)
    {
        SubMenuItemList_.value(i)->Update();
    }

    for(int i=0; i<MenuItemList_.count(); i++)
    {
        MenuItemList_.value(i)->Update();
    }
}

void EC_MenuContainer::PrepareMenuContainer()
{    
    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    setMenuContainerPosition();

    Vector3df position = Vector3df(0.0, 0.0, 0.0);
    float phi;

    //Set menuItem positions in circle.
    for(int i = 0; i < numberOfMenuelements_; i++)
    {
        EC_MenuItem *menuItem = CreateMenuItem();
        if(menuItem)
        {
            phi = 2 * float(i) * Ogre::Math::PI / float(numberOfMenuelements_) + ( 0.5*Ogre::Math::PI);
            position.x = radius_ * cos(phi);
            position.z = radius_ * sin(phi);

            //LogInfo(ToString(position));
            //LogInfo(ToString(phi));
            menuItem->setphi(phi);
            menuItem->SetMenuItemPosition(position);
            MenuItemList_.append(menuItem);
        }
        else
            LogError("Error while creating menu items");
    }
    //Render();
}

void EC_MenuContainer::setMenuContainerPosition()
{
    Scene::Entity *parent = GetParentEntity();
    Scene::EntityPtr avatarCameraPtr = parent->GetScene()->GetEntityByName("AvatarCamera");
    Scene::EntityPtr freeLookCameraPtr = parent->GetScene()->GetEntityByName("FreeLookCamera");

    distance.z=-10;
    distance.y=-1;

    if(avatarCameraPtr)
    {
        Scene::Entity *avatarCamera = avatarCameraPtr.get();
        cameraPlaceable = checked_static_cast<EC_Placeable*>(avatarCamera->GetComponent("EC_Placeable").get());
        //LogInfo("avatarCamera is active");
    }
    else if(freeLookCameraPtr)
    {
        Scene::Entity *freeLookCamera = freeLookCameraPtr.get();
        cameraPlaceable = checked_static_cast<EC_Placeable*>(freeLookCamera->GetComponent("EC_Placeable").get());
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
    QPoint mousePosition;
    mousePosition.setX(mouse->X());
    mousePosition.setY(mouse->Y());

    RaycastResult* result;
    if(renderer_)
    {
        result = renderer_->Raycast(mouse->X(), mouse->Y());
    }

    if (mouse->IsLeftButtonDown() && !ent_clicked_)
    {
        int i=0;
        while(!ent_clicked_ && !subMenu_clicked_ && i<MenuItemList_.count())
        {
            if(result->entity_ == MenuItemList_.at(i)->GetParentEntity())
            {
                ent_clicked_ = true;
                //to stop scrolling when clicked
                speed_ = 0;
            }
            i++;
        }
        if(!ent_clicked_)
        {
            int i=0;
            while(!subMenu_clicked_ && i<SubMenuItemList_.count())
            {
                if(result->entity_ == SubMenuItemList_.at(i)->GetParentEntity())
                {
                    subMenu_clicked_ = true;
                    //to stop scrolling when clicked
                    speed_ = 0;
                }
                i++;
            }

        }
    }

    if(subMenu_clicked_ && mouse->IsLeftButtonDown() && mouse->eventType == MouseEvent::MouseMove)
    {
        Vector3df position = Vector3df(0.0,0.0,0.0);
        for(int i = 0; i<SubMenuItemList_.count(); i++)
        {
            //Sets new angle for components using polar coordinates.
            float phi = SubMenuItemList_.at(i)->getphi() + float((float)mouse->RelativeY()/250);

            //Next position for menu components.
            //In z-angle +radius_ is for moving submenu closer to camera. Otherwise it is inside main circle.
            position.y = subMenuRadius_ * Ogre::Math::Cos(phi);
            position.z = subMenuRadius_ * Ogre::Math::Sin(phi) + radius_;
            SubMenuItemList_.at(i)->setphi(phi);
            /// \todo add submenuselected attribute and set it here.

            SubMenuItemList_.at(i)->SetMenuItemPosition(position);
        }
        //LogInfo("Selected planar: " + ToString(selected_));
        speed_=mouse->RelativeX();

    }

    if(ent_clicked_ && mouse->IsLeftButtonDown() && mouse->eventType == MouseEvent::MouseMove)
    {
        Vector3df position = Vector3df(0.0,0.0,0.0);
        for(int i = 0; i<MenuItemList_.count(); i++)
        {
            //Sets new angle for components using polar coordinates.
            float phi = MenuItemList_.at(i)->getphi() - float((float)mouse->RelativeX()/250);

            //Next position for menu components.
            position.x = radius_ * Ogre::Math::Cos(phi);
            position.z = radius_ * Ogre::Math::Sin(phi);
            MenuItemList_.at(i)->setphi(phi);

            if(Ogre::Math::Sin(phi) > 0.950)
                selected_=i;


            MenuItemList_.at(i)->SetMenuItemPosition(position);
        }
        //LogInfo("Selected planar: " + ToString(selected_));
        speed_=mouse->RelativeX();

    }


    if(mouse->eventType == MouseEvent::MouseReleased && (ent_clicked_ || subMenu_clicked_))
    {
        if(speed_ == 0)
        {
            if(result)
            {
                renderTimer_->stop();
                /// \TODO Lock submenu to one specific menuitem
                /// \todo When scrolling change submenu to selected menuitem
                if(result->entity_ == MenuItemList_.at(selected_)->GetParentEntity() && !subMenu_)
                {
                    subMenu_ = true;
                    createSubMenu(selected_);
                    LogInfo("Open submenu");
                }
                else if(result->entity_ == MenuItemList_.at(selected_)->GetParentEntity() && subMenu_)
                {
                    Scene::SceneManager *sceneManager_ = framework_->Scene()->GetDefaultScene().get();
                    if(!sceneManager_)
                        return;

                    subMenu_ = false;

                    for(int i=0; i<SubMenuItemList_.count(); i++)
                    {
                        //delete submenu entities
                        entity_id_t id = SubMenuItemList_.value(i)->GetParentEntity()->GetId();
                        sceneManager_->RemoveEntity(id, AttributeChange::LocalOnly);
                    }
                    SubMenuItemList_.clear();
                    LogInfo("Close submenu");
                }
                //renderTimer_->start();
            }
        }

        if(speed_>3 || speed_<-3)
        {
            scrollerTimer_Interval=50;
            //LogInfo(ToString(speed_));
            if(subMenu_clicked_)
                subMenuIsScrolling = true;

            scrollerTimer_->setInterval(scrollerTimer_Interval);
            scrollerTimer_->start();
        }
        else
        {
            centerAfterRotation();
            scrollerTimer_->stop();
            speed_=0;
        }
        ent_clicked_ = false;
        subMenu_clicked_ = false;
    }

}

void EC_MenuContainer::kinecticScroller()
{
    if(speed_!=0)
    {
        Vector3df position = Vector3df(0.0,0.0,0.0);

        if(subMenuIsScrolling)
        {
            for(int i=0; i<SubMenuItemList_.count(); i++)
            {
                float phi = SubMenuItemList_.at(i)->getphi() - speed_ * scrollerTimer_Interval/10000;

                position.y = subMenuRadius_ * cos(phi);
                position.z = subMenuRadius_ * sin(phi) + radius_;

                SubMenuItemList_.at(i)->setphi(phi);
                SubMenuItemList_.at(i)->SetMenuItemPosition(position);

                //if(position.z>1.8)
                 //   selected_=i;
            }
        }
        else
        {
            for(int i=0; i<MenuItemList_.count(); i++)
            {
                float phi = MenuItemList_.at(i)->getphi() - speed_ * scrollerTimer_Interval/10000;

                position.x = radius_ * cos(phi);
                position.z = radius_ * sin(phi);

                MenuItemList_.at(i)->setphi(phi);
                MenuItemList_.at(i)->SetMenuItemPosition(position);

                if(Ogre::Math::Sin(phi) > 0.950)
                    selected_=i;
            }
            //LogInfo("Selected planar: " + ToString(selected_));
        }
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
        centerAfterRotation();
        scrollerTimer_->stop();
    }
}

void EC_MenuContainer::centerAfterRotation()
{
    /// \todo This should rotate the whole menu to default position, where closes planar is the selected one.
}

void EC_MenuContainer::createSubMenu(int menuIndex)
{
    //Create submenu here.
    //menuIndex indicates which MenuItem was selected when this function were called.
    if(MenuItemList_.empty())
        return;

    int subItems = MenuItemList_.at(menuIndex)->GetNumberOfSubItems();
    subMenuRadius_=radius_/2.0f;

    for(int i=0; i<subItems; i++)
    {
        EC_MenuItem *menuItem = CreateMenuItem();
        SubMenuItemList_.append(menuItem);
    }


    Vector3df position = Vector3df(0.0, 0.0, 0.0);
    float phi;

    if(SubMenuItemList_.count()>0)
    {
        //Set menuItem positions in circle.
        for(int i = 0; i < SubMenuItemList_.count(); i++)
        {
            phi = 2 * float(i) * Ogre::Math::PI / float(SubMenuItemList_.count()) + ( Ogre::Math::PI);
            position.y = subMenuRadius_ * cos(phi);
            position.z = subMenuRadius_ * sin(phi) + radius_;

            SubMenuItemList_.at(i)->setphi(phi);
            SubMenuItemList_.at(i)->SetMenuItemPosition(position);
        }
    }
    //Render();
}

EC_MenuItem* EC_MenuContainer::CreateMenuItem()
{
    Scene::SceneManager *sceneManager_ = framework_->Scene()->GetDefaultScene().get();

    ComponentPtr parentPlaceable = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false);
    entity_id_t id = sceneManager_->GetNextFreeIdLocal();

    //LogInfo("ID: " + ToString(id));
    Scene::EntityPtr entity_ = sceneManager_->CreateEntity(id, QStringList(), AttributeChange::LocalOnly, false);

    //LogInfo("Pointer " + ToString(entity_));
    if(!entity_)
        LogInfo("Couldn't create entity with given ID");
    else
    {
        Scene::Entity *MenuItemEntity = entity_.get();
        IComponent *iComponent = MenuItemEntity->GetOrCreateComponent("EC_MenuItem", AttributeChange::LocalOnly, false).get();

        if(iComponent)
        {
            EC_MenuItem *menuItem = dynamic_cast<EC_MenuItem*>(iComponent);
            //Sets parent entity for menuItem-entitys placeable component
            menuItem->SetMenuContainerEntity(parentPlaceable);
            sceneManager_->EmitEntityCreated(MenuItemEntity, AttributeChange::LocalOnly);
            return menuItem;
        }
        else
            LogInfo("Error while creating MenuItem");
    }

}

EC_Placeable *EC_MenuContainer::GetOrCreatePlaceableComponent()
{
    if (!GetParentEntity())
        return 0;
    //IComponent *iComponent = parent->GetOrCreateComponentRaw(EC_3DCanvas::TypeNameStatic(), AttributeChange::LocalOnly, false);
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    //IComponent *iComponent = GetParentEntity()->CreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    return placeable;
}

void EC_MenuContainer::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{

}
