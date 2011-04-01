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
#include "UiServiceInterface.h"
#include "UiProxyWidget.h"
#include "EventManager.h"
#include "ModuleManager.h"




std::string QMLUIModule::type_name_static_ = "QMLUIModule";

QMLUIModule::QMLUIModule() :
    IModule(type_name_static_)

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
    mobilitymodule_ = framework_->GetModule<MobilityModule>();

    if (ui)
    {
        window_ = new QMLWidget();

        UiProxyWidget *proxy = new UiProxyWidget(window_, Qt::Widget);
        uis->AddProxyWidgetToScene(proxy);
        proxy->setVisible(true);
        proxy->setFocusPolicy(Qt::NoFocus);

        QObject *QMLUI = dynamic_cast<QObject*>(window_->rootObject());
        QObject::connect(QMLUI, SIGNAL(exit()), this, SLOT(Exit()));

        QObject::connect(mobilitymodule_, SIGNAL(networkStateChanged(MobilityModule::NetworkState)), this, SLOT(NetworkStateChanged(MobilityModule::NetworkState)));
        QObject::connect(this, SIGNAL(giveQMLNetworkState(QVariant)), QMLUI, SLOT(networkstatechanged(QVariant)));

        QObject::connect(mobilitymodule_, SIGNAL(batteryLevelChanged(int)), this, SLOT(BatteryLevelChanged(int)));
        QObject::connect(this, SIGNAL(giveQMLBatteryLevel(QVariant)), QMLUI, SLOT(batterylevelchanged(QVariant)));

        QObject::connect(mobilitymodule_, SIGNAL(usingBattery(bool)), this, SLOT(usingBattery(bool)));
        QObject::connect(this, SIGNAL(giveQMLUsingBattery(QVariant)), QMLUI,SLOT(usingbattery(QVariant)));

        emit giveQMLNetworkState((QVariant)mobilitymodule_->networkState());
        //emit giveQMLUsingBattery(mobilitymodule_->usingBattery());

        //QMLUI->getnetworkstate(mobilitymodule_->networkState());
        //QObject::connect(this, SIGNAL(helloQML(QVariant)), QMLUI, SLOT(xmlfunction(QVariant)));
        //QObject::connect(mobilitymodule_, SIGNAL(networkStateChanged(MobilityModule::NetworkState networkState);

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


void QMLUIModule::Exit()
{
     framework_->Exit();

     //sceneMngr = framework_->Scene()->GetDefaultScene().get();

     /*scenexml_ = sceneMngr->GetSceneXML(true, true);
     sceneMngr->SaveSceneXML(("testingxmlscene.xml"));
     emit helloQML("../../testingxmlscene.xml");*/
}

void QMLUIModule::NetworkStateChanged(MobilityModule::NetworkState state)
{
    QVariant nstate = (QVariant)state;
    emit giveQMLNetworkState(nstate);
}

void QMLUIModule::BatteryLevelChanged(int batterylevel)
{
    QVariant blevel = (QVariant)batterylevel;
    emit giveQMLBatteryLevel(batterylevel);
}

void QMLUIModule::usingBattery(bool uB)
{
    if (uB)
        emit giveQMLUsingBattery((QVariant)1);
    else
        emit giveQMLUsingBattery((QVariant)0);
}

extern "C" void POCO_LIBRARY_API SetProfiler(Foundation::Profiler *profiler);
void SetProfiler(Foundation::Profiler *profiler)
{
    Foundation::ProfilerSection::SetProfiler(profiler);
}

POCO_BEGIN_MANIFEST(IModule)
   POCO_EXPORT_CLASS(QMLUIModule)
POCO_END_MANIFEST

