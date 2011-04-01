
#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "QMLWidget.h"
#include "MemoryLeakCheck.h"


QMLWidget::QMLWidget()
{
    move(0,0);
    setSource(QUrl::fromLocalFile("./data/qmlfiles/QMLUI.qml"));
    //setSource(QUrl::fromLocalFile("../QMLUIModule/qml/QMLUI.qml"));
    setStyleSheet("QDeclarativeView {background-color: transparent;}");
    setWindowState(Qt::WindowFullScreen);
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
}
