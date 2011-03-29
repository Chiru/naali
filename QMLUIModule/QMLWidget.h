/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   QMLWidget.h
 *  @brief  Simple full screen login widget for OpenSim and realXend authentication methods.
 */

#ifndef incl_LoginModule_QMLWidget_h
#define incl_LoginModule_QMLWidget_h

#include "Foundation.h"
#include <QtDeclarative/QtDeclarative>

#include <QTimer>

/// Simple full screen login widget for OpenSim and realXend authentication methods.
class QMLWidget : public QDeclarativeView
{
    Q_OBJECT

public:
    /** Constructor.
     *  @param loginInfo
     */
    explicit QMLWidget(Foundation::Framework *framework);

public slots:
    QMap<QString, QString> GetLoginInfo() const;
    void SetLoginInfo(QMap<QString, QString> info);

//    void StatusUpdate(bool connecting, const QString &message);
    void SetStatus(const QString &message);

    ///
    void Connected();

signals:
    ///
    void Connect(const QMap<QString, QString> &login_data);

    ///
    void ExitClicked();

private slots:
    ///
    void ReadConfig();

    ///
    void WriteConfig();

    ///
    void ParseInputAndConnect();

    ///
    void StartProgressBar();

    ///
    void UpdateProgressBar();

    ///
    void StopProgressBar();

    ///
    void Exit();

private:
    ///
    QTimer *progress_timer_;

    ///
    int progress_direction_;

    ///
    Foundation::Framework *framework_;
};

#endif
