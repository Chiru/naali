
/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 */

#ifndef incl_EC_Menu_MenuDataItem_h
#define incl_EC_Menu_MenuDataItem_h

#include <QList>
#include <QString>
#include <QStringList>

class MenuDataItem
{
public:
    MenuDataItem(MenuDataItem *parent = 0);
    ~MenuDataItem();

    bool AddChildren();
    MenuDataItem* Parent();

    bool SetMeshRef(QString meshref);
    bool SetMaterialRef(QStringList materialref);

    int GetChildCount();
    MenuDataItem* GetChildDataItem(int index);

    QString GetMeshRef();
    QStringList GetMaterialRef();


private:
    MenuDataItem *parent_;
    QList<MenuDataItem*> childItems_;
    QString meshreference_;
    QStringList materialreference_;

};
#endif
