/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_MenuItem.h
 *  @brief  EC_MenuItem is part of 3Dmenu and it is used by EC_MenuContainer.
 *  @note   no notes
 *
 */

#ifndef incl_EC_Menu_EC_MenuItem_h
#define incl_EC_Menu_EC_MenuItem_h

#include "MenuDataItem.h"

#include "IComponent.h"
#include "IAttribute.h"
#include "Declare_EC.h"
//#include "AssetAPI.h"

#include "AssetReference.h"
#include <QString>
#include <QList>
#include <QtGui>

#include "EC_Placeable.h"


//#include <QtDeclarative/QtDeclarative>

/**
<table class="header">
<tr>
<td>
<h2>EC_MenuItem</h2>
EC_MenuItem Component.

Registered by RexLogic::RexLogicModule.

<b>No Attributes.</b>

<b>Exposes the following scriptable functions:</b>
<ul>
<li>...
</ul>

<b>Reacts on the following actions:</b>
<ul>
<li>...
</ul>
</td>
</tr>

</table>
*/

class EC_Mesh;
class EC_3DCanvas;
class EC_Placeable;
class EC_MenuContainer;


class EC_MenuItem : public IComponent
{
    Q_OBJECT
    DECLARE_EC(EC_MenuItem);

private:
    /// Constuctor.
    /// @param module Owner module.
    explicit EC_MenuItem(IModule *module);

    QWidget *widget_;
    int widgetSubmesh_;

    QString meshreference_;
    AssetReferenceList materials_;

    MenuDataItem *itemdata_;

public:
    /// Destructor.
    ~EC_MenuItem();

    Q_PROPERTY(float phi READ getphi WRITE setphi);
    DEFINE_QPROPERTY_ATTRIBUTE(float, phi);

public slots:

    bool OpenSubMenu();

    void SetDataItem(MenuDataItem *dataitemptr);

    MenuDataItem* GetDataItem();

    Vector3df GetMenuItemPosition();

    //! Setter for entity position
    void SetMenuItemPosition(Vector3df position);

    void SetMenuItemVisible();
    void SetMenuItemHidden();

    void SetMenuItemWidget(int subMeshIndex, QWidget *data);

    //! Setter for EC_Placeable parameters
    void SetParentMenuContainer(ComponentPtr);


private slots:
    //! Prepares everything related to the parent widget and other needed components.
    void PrepareMenuItem();

    //! Monitors this entitys added components.
    void ComponentAdded(IComponent *component, AttributeChange::Type change);

    //! Monitors this entitys removed components.
    void ComponentRemoved(IComponent *component, AttributeChange::Type change);

    //! Monitors this components Attribute changes.
    void AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType);

    //! Setter for what mesh to use in this menuitem
    void SetMenuItemMesh(QString, QStringList);

    //! Get parent entitys EC_3DCanvas. Return 0 if not present.
    EC_3DCanvas* GetOrCreateCanvasComponent();

    //! Create as many EC_Mesh components to the parent entity as given in input.
    EC_Mesh* GetOrCreateMeshComponent();

    EC_Placeable* GetOrCreatePlaceableComponent();


signals:
    void OnAttributeChanged(IAttribute*, AttributeChange::Type);

};

#endif
