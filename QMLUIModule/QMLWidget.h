/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   QMLWidget.h
 *  @brief  Simple full screen login widget for OpenSim and realXend authentication methods.
 */

#ifndef incl_LoginModule_QMLWidget_h
#define incl_LoginModule_QMLWidget_h

#include <QtDeclarative/QDeclarativeView>


class QMLWidget : public QDeclarativeView
{
    Q_OBJECT

public:

    explicit QMLWidget();


};

#endif
