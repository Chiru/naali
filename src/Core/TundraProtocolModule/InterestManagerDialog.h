/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 */

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include "IMProperties.h"

class InterestManagerDialog : public QDialog
{
    Q_OBJECT

public:
    InterestManagerDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0, IMProperties *properties = 0);
    ~InterestManagerDialog();

private:
    QCheckBox *cb_isEnabled_;
    QCheckBox *cb_euclidean_;
    QCheckBox *cb_relevance_;
    QCheckBox *cb_raycast_;
    QSpinBox *sb_range_;
    QSpinBox *sb_criticalrange_;
    QSpinBox *sb_raycastrange_;

    bool euclidean_mode_;
    bool relevance_mode_;
    bool raycast_mode_;

    IMProperties *properties_;

private slots:
    void accept();
    void euclidean_checkbox_clicked();
    void raycast_checkbox_clicked();
    void relevance_checkbox_clicked();
    void range_spinbox_clicked(int value);
    void criticalrange_spinbox_clicked(int value);
    void raycastrange_spinbox_clicked(int value);
    void isEnabled_clicked();
};


