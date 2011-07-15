/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 */

#ifndef incl_EC_Menu_MenuDataModel_h
#define incl_EC_Menu_MenuDataModel_h

#include <QString>
#include <QList>
#include <QStringList>
#include <QObject>

class MenuDataItem;

class MenuDataModel : public QObject
{
    Q_OBJECT
public:
    MenuDataModel(QObject *parent = 0);
    ~MenuDataModel();

public slots:
    bool AddItem(MenuDataItem*);
    bool AddItem(QString mesh, QStringList materials);

    bool AddItemToIndex(MenuDataItem *item, int index=0);
    bool AddItemToIndex(QString mesh, QStringList materials, int index=0);

    MenuDataItem* GetMenuDataItem(int index);
    int GetNumberOfDataItems();

    MenuDataItem* GetParent();

private:
    MenuDataItem *parent_;
    QList<MenuDataItem*> menudataitems_;

};

#endif
