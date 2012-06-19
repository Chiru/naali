/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 */

#include "StableHeaders.h"
#include "InterestManagerDialog.h"
#include <QLabel>
#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>


InterestManagerDialog::InterestManagerDialog(QWidget *parent, Qt::WindowFlags flags, IMProperties *properties)
    : QDialog(parent, flags)
{
    setModal(true);
    setWindowTitle(tr("Interest Manager Properties"));
    setAttribute(Qt::WA_DeleteOnClose);
    QApplication::setStyle("windows");
    if(properties != NULL)
    {
        properties_ = properties;
        euclidean_mode_ = properties_->GetEuclideanMode();
        relevance_mode_ = properties_->GetRelevanceMode();
        raycast_mode_ = properties_->GetRaycastMode();
    }

    sb_range_ = new QSpinBox();
    sb_range_->setRange(0, 100);
    sb_range_->setSingleStep(1);
    sb_range_->setValue((properties_ == NULL ? 20 : properties_->GetMaxRange()));
    sb_range_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    sb_criticalrange_ = new QSpinBox();
    sb_criticalrange_->setRange(0, 100);
    sb_criticalrange_->setSingleStep(1);
    sb_criticalrange_->setValue((properties_ == NULL ? 10 : properties_->GetCriticalRange()));
    sb_criticalrange_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    sb_raycastrange_ = new QSpinBox();
    sb_raycastrange_->setRange(0, 100);
    sb_raycastrange_->setSingleStep(1);
    sb_raycastrange_->setValue((properties_ == NULL ? 10 : properties_->GetRaycastRange()));
    sb_raycastrange_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton *buttonOK = new QPushButton(tr("Apply"));
    QPushButton *buttonCancel = new QPushButton(tr("Close"));

    buttonOK->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    buttonOK->setDefault(true);
    buttonCancel->setAutoDefault(false);

    cb_isEnabled_ = new QCheckBox(this);
    cb_isEnabled_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cb_isEnabled_->setText(tr("Filtering &Enabled"));
    cb_isEnabled_->setChecked((properties_ == NULL ? false : properties_->isEnabled()));

    cb_euclidean_ = new QCheckBox(this);
    cb_euclidean_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cb_euclidean_->setText(tr("Enabled"));
    cb_euclidean_->setChecked((properties_ == NULL ? false : properties_->GetEuclideanMode()));

    cb_raycast_ = new QCheckBox(this);
    cb_raycast_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cb_raycast_->setText(tr("Enabled"));
    cb_raycast_->setChecked((properties_ == NULL ? false : properties_->GetRaycastMode()));

    cb_relevance_ = new QCheckBox(this);
    cb_relevance_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cb_relevance_->setText(tr("Re&levance Filtering"));
    cb_relevance_->setChecked((properties_ == NULL ? false : properties_->GetRelevanceMode()));

    if(cb_isEnabled_->isChecked() == false)
    {
        cb_euclidean_->setEnabled(false);
        cb_relevance_->setEnabled(false);
        cb_raycast_->setEnabled(false);
        sb_criticalrange_->setEnabled(false);
        sb_raycastrange_->setEnabled(false);
        sb_range_->setEnabled(false);
    }

    if(euclidean_mode_ || raycast_mode_)
        cb_relevance_->setEnabled(true);
    else
        cb_relevance_->setEnabled(false);

    QLabel *lRange = new QLabel(tr("Radius"));
    QLabel *leRadius = new QLabel(tr("Radius"));
    QLabel *lrRadius = new QLabel(tr("Radius"));

    QGroupBox *gb_eucl = new QGroupBox(tr("Eucl. Dist. Filter"));
    QGridLayout *gb_egrid = new QGridLayout();
    gb_egrid->setVerticalSpacing(8);
    gb_egrid->addWidget(cb_euclidean_, 1, 0, Qt::AlignLeft, 1);
    gb_egrid->addWidget(sb_criticalrange_, 2, 0, Qt::AlignLeft, 1);
    gb_egrid->addWidget(leRadius, 2, 1, Qt::AlignLeft, 1);
    gb_eucl->setLayout(gb_egrid);

    QGroupBox *gb_ray = new QGroupBox(tr("Ray Visibility Filter"));
    QGridLayout *gb_rgrid = new QGridLayout();
    gb_rgrid->setVerticalSpacing(8);
    gb_rgrid->addWidget(cb_raycast_, 1, 0, Qt::AlignLeft, 1);
    gb_rgrid->addWidget(sb_raycastrange_, 2, 0, Qt::AlignLeft, 1);
    gb_rgrid->addWidget(lrRadius, 2, 1, Qt::AlignLeft, 1);
    gb_ray->setLayout(gb_rgrid);

    QGroupBox *gb_modifiers = new QGroupBox(tr("Modifiers"));
    QGridLayout *gb_mgrid = new QGridLayout();
    gb_mgrid->setVerticalSpacing(8);
    gb_mgrid->addWidget(cb_relevance_, 1, 0, Qt::AlignLeft, 1);
    gb_mgrid->addWidget(sb_range_, 2, 0, Qt::AlignLeft, 1);
    gb_mgrid->addWidget(lRange, 2, 1, Qt::AlignLeft, 1);
    gb_modifiers->setLayout(gb_mgrid);

    QGridLayout *grid = new QGridLayout();
    grid->setVerticalSpacing(8);
    grid->addWidget(cb_isEnabled_, 1, 0, Qt::AlignLeft, 1);
    grid->addWidget(gb_eucl, 2, 0, Qt::AlignLeft, 1);
    grid->addWidget(gb_ray, 2, 1, Qt::AlignLeft, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(buttonOK);
    buttonLayout->addWidget(buttonCancel);

    QVBoxLayout *vertLayout = new QVBoxLayout();
    vertLayout->addLayout(grid);
    vertLayout->addWidget(gb_modifiers);
    vertLayout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding));
    vertLayout->addLayout(buttonLayout);

    setLayout(vertLayout);

    connect(buttonOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(cb_isEnabled_, SIGNAL(clicked()), this, SLOT(isEnabled_clicked()));
    connect(cb_euclidean_, SIGNAL(clicked()), this, SLOT(euclidean_checkbox_clicked()));
    connect(cb_raycast_, SIGNAL(clicked()), this, SLOT(raycast_checkbox_clicked()));
    connect(cb_relevance_, SIGNAL(clicked()), this, SLOT(relevance_checkbox_clicked()));
    connect(sb_range_, SIGNAL(valueChanged(int)), this, SLOT(range_spinbox_clicked(int)));
    connect(sb_criticalrange_, SIGNAL(valueChanged(int)), this, SLOT(criticalrange_spinbox_clicked(int)));
    connect(sb_raycastrange_, SIGNAL(valueChanged(int)), this, SLOT(raycastrange_spinbox_clicked(int)));

}

InterestManagerDialog::~InterestManagerDialog()
{

}

void InterestManagerDialog::accept()
{
    properties_->SetEuclideanMode(cb_euclidean_->isChecked());
    properties_->SetRelevanceMode(cb_relevance_->isChecked());
    properties_->SetRaycastMode(cb_raycast_->isChecked());
    properties_->SetRaycastRange(sb_raycastrange_->value());
    properties_->SetMaximumRange(sb_range_->value());
    properties_->SetCriticalRange(sb_criticalrange_->value());
    properties_->setEnabled(cb_isEnabled_->isChecked());
}

void InterestManagerDialog::euclidean_checkbox_clicked()
{
    if(cb_euclidean_->isChecked())
    {
        cb_relevance_->setEnabled(true);
        sb_criticalrange_->setEnabled(true);

        if(cb_relevance_->isChecked() && cb_relevance_->isEnabled())
            sb_range_->setEnabled(true);
    }

    else
    {
        if(!cb_raycast_->isChecked())
        {
            cb_relevance_->setEnabled(false);
            sb_range_->setEnabled(false);
        }
        sb_criticalrange_->setEnabled(false);
    }
}

void InterestManagerDialog::raycast_checkbox_clicked()
{
    if(cb_raycast_->isChecked())
    {
        cb_relevance_->setEnabled(true);
        sb_raycastrange_->setEnabled(true);

        if(cb_relevance_->isChecked() && cb_relevance_->isEnabled())
            sb_range_->setEnabled(true);
    }
    else
    {
        if(!cb_euclidean_->isChecked())
        {
            cb_relevance_->setEnabled(false);
            sb_range_->setEnabled(false);
        }
        sb_raycastrange_->setEnabled(false);
    }
}

void InterestManagerDialog::relevance_checkbox_clicked()
{
    if(cb_relevance_->isChecked())
        sb_range_->setEnabled(true);
    else
        sb_range_->setEnabled(false);
}

void InterestManagerDialog::isEnabled_clicked()
{
    if(cb_isEnabled_->isChecked())
    {
        cb_euclidean_->setEnabled(true);
        cb_raycast_->setEnabled(true);

        if(cb_euclidean_->isChecked() || cb_raycast_->isChecked())
            cb_relevance_->setEnabled(true);

        if(cb_relevance_->isChecked() && cb_relevance_->isEnabled())
            sb_range_->setEnabled(true);
    }
    else
    {
        cb_euclidean_->setEnabled(false);
        cb_relevance_->setEnabled(false);
        cb_raycast_->setEnabled(false);
        sb_criticalrange_->setEnabled(false);
        sb_raycastrange_->setEnabled(false);
        sb_range_->setEnabled(false);
    }

}

void InterestManagerDialog::range_spinbox_clicked(int value)
{
    if(sb_criticalrange_->value() > value)
        sb_criticalrange_->setValue(value);

    if(sb_raycastrange_->value() > value)
        sb_raycastrange_->setValue(value);
}

void InterestManagerDialog::criticalrange_spinbox_clicked(int value)
{
    if(sb_range_->value() < value)
        sb_range_->setValue(value);
}

void InterestManagerDialog::raycastrange_spinbox_clicked(int value)
{
    if(sb_range_->value() < value)
        sb_range_->setValue(value);
}
