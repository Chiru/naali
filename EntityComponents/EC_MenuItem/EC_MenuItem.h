/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_MenuItem.h
 *  @brief  EC_MenuItem is part of 3Dmenu and it is used by EC_MenuContainer.
 *  @note   no notes
 *
 */

#ifndef incl_EC_MenuItem_EC_MenuItem_h
#define incl_EC_MenuItem_EC_MenuItem_h

#include "IComponent.h"
#include "IAttribute.h"
#include "Declare_EC.h"

//#include "SceneFwd.h"
//#include "InputFwd.h"

#include <QString>
#include <QList>

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

class EC_MenuItem : public IComponent
{
    Q_OBJECT
    DECLARE_EC(EC_MenuItem);

private:
    /// Constuctor.
    /// @param module Owner module.
    explicit EC_MenuItem(IModule *module);

public:
    /// Destructor.
    ~EC_MenuItem();

    //! Rendering target submesh index.
    Q_PROPERTY(int renderSubmeshIndex READ getrenderSubmeshIndex WRITE setrenderSubmeshIndex);
    DEFINE_QPROPERTY_ATTRIBUTE(int, renderSubmeshIndex);

    //! Boolean for interactive mode, if true it will show context menus on mouse click events.
    Q_PROPERTY(bool interactive READ getinteractive WRITE setinteractive);
    DEFINE_QPROPERTY_ATTRIBUTE(bool, interactive);

    Q_PROPERTY(float phi READ getphi WRITE setphi);
    DEFINE_QPROPERTY_ATTRIBUTE(float, phi);

public slots:
    //! Setter for EC_Placeable parameters
    void SetMenuContainerEntity(ComponentPtr);

    //! Setter for entity position
    void SetMenuItemPosition(Vector3df);

    void SetMenuItemWidget(int, QWidget*);

    //! Setter for what mesh to use in this menuitem
    void SetMenuItemMesh(QString, QStringList);

    void Update();


private slots:
    //! Prepares everything related to the parent widget and other needed components.
    void PrepareMenuItem();

    //! Monitors this entitys added components.
    void ComponentAdded(IComponent *component, AttributeChange::Type change);

    //! Monitors this entitys removed components.
    void ComponentRemoved(IComponent *component, AttributeChange::Type change);

    //! Monitors this components Attribute changes.
    void AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType);

    //! Create as many EC_Mesh components to the parent entity as given in input.
    EC_Mesh* GetOrCreateMeshComponent();

    //! Get parent entitys EC_3DCanvas. Return 0 if not present.
    EC_3DCanvas* GetOrCreateCanvasComponent();

    EC_Placeable* GetOrCreatePlaceableComponent();

    //! Handles entity action WebViewControllerChanged
    /// \note The action signature is (string)"WebViewControllerChanged", (int)"id", (string)"name"
    void ActionControllerChanged(QString id, QString newController);


signals:
    void OnAttributeChanged(IAttribute*, AttributeChange::Type);

};

#endif
