/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file
 *  @brief
 */

#include "StableHeaders.h"
#include "DebugOperatorNew.h"

#include "QMLUIModule.h"
#include "QMLWidget.h"

#include "UiServiceInterface.h"
#include "UiService.h"
#include "LoginServiceInterface.h"
#include "UiServiceInterface.h"
#include "UiProxyWidget.h"
#include "EventManager.h"
//#include "SceneManager.h"
#include "SceneAPI.h"
#include "Entity.h"


#ifdef ENABLE_TAIGA_LOGIN
#include "../ProtocolUtilities/NetworkEvents.h"
#endif

#include "../TundraLogicModule/TundraEvents.h"

#include "MemoryLeakCheck.h"
#include <QtOpenGL/QtOpenGL>


std::string QMLUIModule::type_name_static_ = "QMLUIModule";

QMLUIModule::QMLUIModule() :
    IModule(type_name_static_),
    window_(0),
    framework_category_(0),
    network_category_(0),
    connected_(false)
{
    //For QML-debugging
    QByteArray data = "1";
    qputenv("QML_IMPORT_TRACE", data);
}

QMLUIModule::~QMLUIModule()
{
}

void QMLUIModule::PreInitialize()
{
}

void QMLUIModule::Initialize()
{
}

void QMLUIModule::PostInitialize()
{

    UiServiceInterface *ui = framework_->GetService<UiServiceInterface>();
    UiService *uis = framework_->GetService<UiService>();

    if (ui)
    {
        window_ = new QMLWidget(framework_);

        UiProxyWidget *proxy = new UiProxyWidget(window_, Qt::Widget);
        uis->AddProxyWidgetToScene(proxy);
        proxy->setVisible(true);
        proxy->setFocusPolicy(Qt::NoFocus);

        QObject *QMLUI = dynamic_cast<QObject*>(window_->rootObject());
        QObject::connect(QMLUI, SIGNAL(exit()), this, SLOT(Exit()));
        //QObject::connect(this, SIGNAL(helloQML(QVariant)), QMLUI, SLOT(xmlfunction(QVariant)));

        /*QGLPixelBuffer b(window_->size());
        QPainter p(&b);
        window_->render(&p);
        b.toImage().save("lol.png");*/
        //QPixmap p(window_->size()); QPainter painter(&p); window_->render(&painter);p.save("ollitest.png");


        /*QDeclarativeView *teaView = new QDeclarativeView;
        teaView->setViewport(new QGLWidget);
        teaView->setSource(QUrl::fromLocalFile("../QMLUIModule/teapot.qml"));
        teaView->show();

        QPixmap d(teaView->size()); QPainter painterr(&d); teaView->render(&painterr);d.save("teatest.png");*/

        /*QGLPixelBuffer b(teaView->size());
        QPainter p(&b);
        teaView->render(&p);
        b.toImage().save("pbteatest.png");*/
        //QPixmap p(window_->size()); QPainter painter(&p); window_->render(&painter);p.save("ollitest.png");

        /*QDeclarativeEngine *engine = new QDeclarativeEngine;
        QDeclarativeComponent component(engine, (QUrl::fromLocalFile("/home/olli/src/git/010311/chiru/QMLUIModule/teapot.qml")));
        QGraphicsObject *object = qobject_cast<QGraphicsObject*>(component.create());
        uis->GetScene("scene_")->addItem(object);

        QDeclarativeView *tea = new QDeclarativeView;
        tea->setSource(QUrl::fromLocalFile("/home/olli/src/git/010311/chiru/QMLUIModule/teapot.qml"));
        ui->AddWidgetToScene(tea, Qt::Dialog);
        ui->ShowWidget(tea);*/



    }
}

void QMLUIModule::qmlTest(UiServiceInterface *ui, QDeclarativeView *view)
{
    ui->ShowWidget(view);

}

void QMLUIModule::Uninitialize()
{
    SAFE_DELETE(window_);
}

void QMLUIModule::Update(f64 frametime)
{
    RESETPROFILER;
}

// virtual
bool QMLUIModule::HandleEvent(event_category_id_t category_id, event_id_t event_id, IEventData* data)
{    
    return false;
}

void QMLUIModule::HandleKeyEvent(KeyEvent *key)
{
}

void QMLUIModule::ProcessTundraLogin(const QMap<QString, QString> &data)
{
    if (data["AvatarType"] == "Tundra")
    {

        std::string worldAddress = data["WorldAddress"].toStdString();
        unsigned short port = 0; // Use default if not specified

        size_t pos = worldAddress.find(':');
        if (pos != std::string::npos)
        {
            try
            {
                port = ParseString<int>(worldAddress.substr(pos + 1));
            }
            catch (...) {}
            worldAddress = worldAddress.substr(0, pos);
        }

        TundraLogic::Events::TundraLoginEventData logindata;
        logindata.address_ = worldAddress;
        logindata.port_ = port;
        logindata.username_ = data["Username"].toStdString();
        logindata.password_ = data["Password"].toStdString();
        logindata.protocol_ = data["Protocol"].toStdString();
        LogInfo("Attempting Tundra connection to " + worldAddress + " as " + logindata.username_);
        framework_->GetEventManager()->SendEvent(tundra_category_, TundraLogic::Events::EVENT_TUNDRA_LOGIN, &logindata);
    }
}

void QMLUIModule::Exit()
{
     framework_->Exit();
     /*sceneMngr = framework_->Scene()->GetDefaultScene().get();
     scenexml_ = sceneMngr->GetSceneXML(true, true);
     sceneMngr->SaveSceneXML(("testingxmlscene.xml"));
     emit helloQML("../../testingxmlscene.xml");*/

}

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}

POCO_BEGIN_MANIFEST(IModule)
   POCO_EXPORT_CLASS(QMLUIModule)
POCO_END_MANIFEST

