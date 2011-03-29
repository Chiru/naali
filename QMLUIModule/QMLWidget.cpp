/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   QMLWidget.cpp
 *  @brief  Simple full screen login widget for OpenSim and realXend authentication methods.
 */

#include "StableHeaders.h"
#include "DebugOperatorNew.h"
#include "QMLWidget.h"
#include "MemoryLeakCheck.h"

#include <QPixmap>

QMLWidget::QMLWidget(Foundation::Framework *framework) :
    framework_(framework),
    progress_timer_(new QTimer(this))
{
    move(0,0);
    setSource(QUrl::fromLocalFile("./data/qmlfiles/QMLUI.qml"));
    setStyleSheet("QDeclarativeView {background-color: transparent;}");
    setWindowState(Qt::WindowFullScreen);
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
}
