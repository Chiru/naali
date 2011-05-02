/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Template.cpp
 *  @brief  EC_Template is empty template for EC components.
 *          This componen can be used as a template when creating new EC components.
 *  @note   no notes
 */


#include "StableHeaders.h"
#include "EC_Menu.h"
#include "EC_3DCanvas.h"
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

DEFINE_POCO_LOGGING_FUNCTIONS("EC_Menu")

EC_Menu::EC_Menu(IModule *module) :
    IComponent(module->GetFramework()),
    renderSubmeshIndex(this, "Render Submesh", 0),
    interactive(this, "Interactive", false),
    ent_clicked_(false),
    save_start_position_(true),
    numberOfMenuelements_(10),
    radius_(2.0)
{
    renderTimer_ = new QTimer();
    scrollerTimer_ = new QTimer();
    // Connect signals from IComponent
    connect(this, SIGNAL(ParentEntitySet()), SLOT(PrepareMenu()), Qt::UniqueConnection);
    connect(this, SIGNAL(OnAttributeChanged(IAttribute*, AttributeChange::Type)), SLOT(AttributeChanged(IAttribute*, AttributeChange::Type)), Qt::UniqueConnection);
    QObject::connect(renderTimer_, SIGNAL(timeout()), this, SLOT(Render()));
    QObject::connect(scrollerTimer_, SIGNAL(timeout()), this, SLOT(kinecticScroller()));

    renderTimer_->setInterval(40);
    renderTimer_->start();


    renderer_ = module->GetFramework()->GetService<Foundation::RenderServiceInterface>();

    //Create a new input context that menu will use to fetch input.
    input_ = module->GetFramework()->Input()->RegisterInputContext("MenuInput", 100);

    // To be sure that Qt doesn't play tricks on us and miss a mouse release when we're in FPS mode,
    // grab the mouse movement input over Qt.
    input_->SetTakeMouseEventsOverQt(true);

    // Listen on mouse input signals.
    connect(input_.get(), SIGNAL(OnMouseEvent(MouseEvent *)), this, SLOT(HandleMouseInputEvent(MouseEvent *)));

    //LogInfo("EC_Menu initialized");
}

EC_Menu::~EC_Menu()
{
    /// \todo Write your own EC_Component destructor here

    SAFE_DELETE_LATER(listview_);
}


void EC_Menu::PrepareMenu()
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

    // Create EC_Mesh components.
    /// \TODO When creating menu, this should check number of menuelements and give that as a imput to CreateMeshComponents().
    MeshList_ = CreateMeshComponents(numberOfMenuelements_);
    if (MeshList_.empty())
    {
        // Wait for EC_Mesh to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }
    else
    {
        Vector3df position = Vector3df(0.0, 0.0, 0.0);
        //initializes meshes in circle

        float phi;
        for(int i = 0; i < MeshList_.count(); i++)
        {

            //position.x=radius_ * Ogre::Math::Cos( i * radius_ * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
            //position.z=radius_ * Ogre::Math::Sin( i * radius_ * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );

            phi = 2 * float(i) * Ogre::Math::PI / float(MeshList_.count()) + ( 0.5*Ogre::Math::PI);
            phiList.append(phi);

            position.x = radius_ * cos(phiList.at(i));
            position.z = radius_ * sin(phiList.at(i));

            //LogInfo("position.x = " + ToString(position.x) + " position.z = " + ToString(position.z));

            //MeshList_.at(i)->SetName("meshname"+i);
            MeshList_.at(i)->setmeshRef("local://rect_plane.mesh");
            MeshList_.at(i)->SetAdjustPosition(position);

        }
    }

    EC_Placeable *placeable = GetOrCreatePlaceableComponent();
    if (!placeable)
    {
        // Wait for EC_Placeable to be added.
        connect(parent, SIGNAL(ComponentAdded(IComponent*, AttributeChange::Type)), SLOT(ComponentAdded(IComponent*, AttributeChange::Type)), Qt::UniqueConnection);
        return;
    }

    // Get or create local EC_3DCanvas component
    CanvasList_ = CreateSceneCanvasComponents(numberOfMenuelements_);
    if (CanvasList_.count()==0)
    {
        LogError("PrepareComponent: Could not get or create EC_3DCanvas component!");
        return;
    }
    else
    {
        /*view_ = new QDeclarativeView;
        view_->setSource(QUrl::fromLocalFile("./data/qmlfiles/testi.qml"));
        view_->setFixedSize(400, 400);*/

        QStringList TestList;
        QStringList TestList1;
        QStringList TestList2;
        QStringList TestList3;
        QStringList TestList4;
        QStringList TestList5;
        QStringList TestList6;
        QStringList TestList7;
        QStringList TestList8;
        QStringList TestList9;
        TestList<<"list1"<<"list2"<<"list3"<<"list4"<<"list5"<<"list6"<<"list7"<<"list8"<<"list9"<<"list10"<<"list11"<<"list12";
        TestList1.append("1");
        TestList2.append("2");
        TestList3.append("3");
        TestList4.append("4");
        TestList5.append("5");
        TestList6.append("6");
        TestList7.append("7");
        TestList8.append("8");
        TestList9.append("9");
        listview_ = new QListView();

        QListView *listview_1 = new QListView();
        QListView *listview_2 = new QListView();
        QListView *listview_3 = new QListView();
        QListView *listview_4 = new QListView();
        QListView *listview_5 = new QListView();
        QListView *listview_6 = new QListView();
        QListView *listview_7 = new QListView();
        QListView *listview_8 = new QListView();
        QListView *listview_9 = new QListView();

        //QWidget *testiw = new QWidget();
        //QLabel *testl = new QLabel("plaeh", testiw);

        //LogInfo("setModel");
        listview_->setModel(new QStringListModel(TestList));

        listview_1->setModel(new QStringListModel(TestList1));
        listview_2->setModel(new QStringListModel(TestList2));
        listview_3->setModel(new QStringListModel(TestList3));
        listview_4->setModel(new QStringListModel(TestList4));
        listview_5->setModel(new QStringListModel(TestList5));
        listview_6->setModel(new QStringListModel(TestList6));
        listview_7->setModel(new QStringListModel(TestList7));
        listview_8->setModel(new QStringListModel(TestList8));
        listview_9->setModel(new QStringListModel(TestList9));

        SetEntityPosition();
        //LogInfo("SetWidget");

        for(int i=0; i<CanvasList_.count();i++)
        {
            CanvasList_.at(i)->SetMesh(MeshList_.at(i));
            CanvasList_.at(i)->SetSubmesh(0);
            //CanvasList_.at(i)->SetWidget(listview_);
        }
        CanvasList_.at(0)->SetWidget(listview_);
        CanvasList_.at(1)->SetWidget(listview_1);
        CanvasList_.at(2)->SetWidget(listview_2);
        CanvasList_.at(3)->SetWidget(listview_3);
        CanvasList_.at(4)->SetWidget(listview_4);
        CanvasList_.at(5)->SetWidget(listview_5);
        CanvasList_.at(6)->SetWidget(listview_6);
        CanvasList_.at(7)->SetWidget(listview_7);
        CanvasList_.at(8)->SetWidget(listview_8);
        CanvasList_.at(9)->SetWidget(listview_9);
    }
}

void EC_Menu::HandleMouseInputEvent(MouseEvent *mouse)
{
    QPoint mousePosition;
    mousePosition.setX(mouse->X());
    mousePosition.setY(mouse->Y());

    RaycastResult* result;
    if (renderer_)
    {
        result = renderer_->Raycast(mouse->X(), mouse->Y());
        if(result->entity_==GetParentEntity() && mouse->IsLeftButtonDown() && save_start_position_)
        {
            startPosition_.setX(mouse->X());
            startPosition_.setY(mouse->Y());
            save_start_position_ = false;
            ent_clicked_ = true;
            speed_=0; //to stop scrolling when clicked
        }
    }

    if(ent_clicked_ && mouse->IsLeftButtonDown() && mouse->eventType == MouseEvent::MouseMove)
    {

        Vector3df position = Vector3df(0.0,0.0,0.0);
        for(int i=0;i<MeshList_.count();i++)
        {
            //Sets new angle for components using polar coordinates.
            float phi = phiList.at(i) - float((float)mouse->RelativeX()/250);

            phiList.replace(i, phi);
            position.x = radius_ * cos(phiList.at(i));
            position.z = radius_ * sin(phiList.at(i));

            /// \!TODO just a testhack.. need to be changed asap.
            if(position.z>1.8)
                selected_=i;

            MeshList_.at(i)->SetAdjustPosition(position);
        }
        //LogInfo("Selected planar: " + ToString(selected_));
        speed_=mouse->RelativeX();


    }
    if(mouse->eventType == MouseEvent::MouseReleased && ent_clicked_)
    {

        ent_clicked_ = false;
        save_start_position_ = true;

        //acceleratorVector_.setY(mouse->RelativeY());

        if(speed_>3 || speed_<-3)
        {
            scrollerTimer_Interval=50;
            //LogInfo(ToString(speed_));

            scrollerTimer_->setInterval(scrollerTimer_Interval);
            scrollerTimer_->start();
        }
        else
        {
            centerAfterRotation();
            scrollerTimer_->stop();
            speed_=0;
        }
    }

}

void EC_Menu::centerAfterRotation()
{
    //Rotate scroller so, that selected component is in the front.
    //Original positions. We want that planars are in those positions after scrolling is over.
    /*
        position.x=2 * cos( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
        position.z=2 * sin( i * 2 * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
    */

    /// \!TODO Make this work more smoothly
    Vector3df tempPosition = Vector3df(0.0,0.0,0.0);

    tempPosition.x = radius_ * cos( 0.5*Ogre::Math::PI );
    tempPosition.z = radius_ * sin( 0.5*Ogre::Math::PI );
    MeshList_.at(selected_)->SetAdjustPosition(tempPosition);

    //LogInfo("Selected planar: " + ToString(selected_));
    int j = selected_;
    for (int i=1; i<MeshList_.count(); i++)
    {
        j++;

        if(j==MeshList_.count())
            j=0;

        tempPosition.x = radius_ * cos( i * radius_ * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );
        tempPosition.z = radius_ * sin( i * radius_ * Ogre::Math::PI / MeshList_.count() + ( 0.5*Ogre::Math::PI) );

        MeshList_.at(j)->SetAdjustPosition(tempPosition);
    }
}

void EC_Menu::kinecticScroller()
{

    if(speed_!=0)
    {
        Vector3df position = Vector3df(0.0,0.0,0.0);

        for(int i=0;i<MeshList_.count();i++)
        {
            float phi = phiList.at(i) - speed_ * scrollerTimer_Interval/10000;
            phiList.replace(i, phi);
            position.x = radius_ * cos(phiList.at(i));
            position.z = radius_ * sin(phiList.at(i));

            MeshList_.at(i)->SetAdjustPosition(position);

            /// \!TODO just a testhack.. need to be changed asap.
            if(position.z>1.8)
                selected_=i;
        }
        if(speed_<0)
            speed_+=1.0;
        else
            speed_-=1.0;

        //LogInfo("speed: " + ToString(speed_));
    }

    else
    {
        centerAfterRotation();
        scrollerTimer_->stop();
    }

}

void EC_Menu::AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType)
{
}


void EC_Menu::Render()
{
    //LogInfo("Trying to render");

    // Don't do anything if rendering is not enabled
    if (!ViewEnabled() || GetFramework()->IsHeadless())
        return;

    for(int i = 0; i < CanvasList_.count(); i++)
    {
        CanvasList_.at(i)->SetSubmesh(0);
        CanvasList_.at(i)->Update();
    }

}


/*void EC_Menu::ComponentAdded(IComponent *component, AttributeChange::Type change)
{
    LogInfo("In ComponentAdded -slot");
    if(component->TypeName()==EC_Mesh::TypeNameStatic())
    {
        GetOrCreateMeshComponent()->SetName("Testi");
        GetOrCreateMeshComponent()->setmeshRef("local://screen.mesh");
        GetOrCreateMeshComponent()->SetTemporary(true);
    }
    else if (component->TypeName()==EC_Placeable::TypeNameStatic())
    {
        SetEntityPosition();
    }
}*/


void EC_Menu::SetEntityPosition()
{
    Scene::Entity *parent = GetParentEntity();
    Scene::EntityPtr avatarCameraPtr = parent->GetScene()->GetEntityByName("AvatarCamera");
    Scene::EntityPtr freeLookCameraPtr = parent->GetScene()->GetEntityByName("FreeLookCamera");

    distance.z=-10;
    distance.y=-1;

    if(avatarCameraPtr)
    {
        Scene::Entity *avatarCamera = avatarCameraPtr.get();
        //EC_Placeable *cameraPlaceable = checked_static_cast<EC_Placeable*>(avatarCamera->GetComponent("EC_Placeable").get());
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

        GetOrCreatePlaceableComponent()->settransform(entityTransform);
    }
    else
        LogError("Couldn't get OgreCamera Placeable");

}

QList<EC_Mesh *> EC_Menu::CreateMeshComponents(int NumberOfMenuObjects)
{

    if (GetParentEntity())
    {
        for(int i=0; i<NumberOfMenuObjects; i++)
        {
            IComponent *iComponent =  GetParentEntity()->CreateComponent("EC_Mesh", AttributeChange::LocalOnly, false).get();
            EC_Mesh *mesh = dynamic_cast<EC_Mesh*>(iComponent);
            MeshList_.append(mesh);
        }
    }
    else
        LogError("Couldn't get parent entity, returning empty QList");
    return MeshList_;
}

QList<EC_3DCanvas *>EC_Menu::CreateSceneCanvasComponents(int NumberOfMenuObjects)
{
    //if (!GetParentEntity())
        //return 0;
    //IComponent *iComponent = parent->GetOrCreateComponentRaw(EC_3DCanvas::TypeNameStatic(), AttributeChange::LocalOnly, false);
    for(int i=0; i<NumberOfMenuObjects; i++)
    {
        IComponent *iComponent = GetParentEntity()->CreateComponent("EC_3DCanvas", AttributeChange::LocalOnly, false).get();
        EC_3DCanvas *canvas = dynamic_cast<EC_3DCanvas*>(iComponent);
        CanvasList_.append(canvas);
    }

    /// \!TODO some error handling would be nice..
    return CanvasList_;
}

EC_Placeable *EC_Menu::GetOrCreatePlaceableComponent()
{
    if (!GetParentEntity())
        return 0;
    //IComponent *iComponent = parent->GetOrCreateComponentRaw(EC_3DCanvas::TypeNameStatic(), AttributeChange::LocalOnly, false);
    IComponent *iComponent = GetParentEntity()->GetOrCreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    //IComponent *iComponent = GetParentEntity()->CreateComponent("EC_Placeable", AttributeChange::LocalOnly, false).get();
    EC_Placeable *placeable = dynamic_cast<EC_Placeable*>(iComponent);
    return placeable;
}

void EC_Menu::ComponentRemoved(IComponent *component, AttributeChange::Type change)
{

}
