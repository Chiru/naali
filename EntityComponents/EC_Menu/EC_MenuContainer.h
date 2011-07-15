/**
 *  Copyright (c) 2011 CIE / University of Oulu, All Rights Reserved
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_MenuContainer.h
 *  @brief  EC_MenuContainer creates 3D Menu in to scene.
 *          It uses MenuDataModel as a data storage and each visible item in menu is created by EC_MenuItem.
 *  @note   no notes
 *
 */

#ifndef incl_EC_Menu_EC_MenuContainer_h
#define incl_EC_Menu_EC_MenuContainer_h

#include "IComponent.h"
#include "IAttribute.h"
#include "Declare_EC.h"

#include "SceneFwd.h"
#include "InputFwd.h"

#include "CoreTypes.h"

#include "SceneManager.h"
#include "SceneAPI.h"

#include <QString>
#include <QSize>
#include <QPointer>
#include <QPoint>
#include <QTimer>
#include <QMenu>
#include <QList>
#include <QPointer>

#include "EC_Placeable.h"

/**
<table class="header">
<tr>
<td>
<h2>EC_MenuContainer</h2>
EC_MenuContainer Component. This component creates subentities with EC_MenuItem component and is parent entity for those.

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

class EC_MenuItem;
class MenuDataModel;
class EC_Placeable;
class EC_OgreCamera;
class QListView;
class QMouseEvent;
class RaycastResult;

class EC_MenuContainer : public IComponent
{
    Q_OBJECT
    DECLARE_EC(EC_MenuContainer);

private:
    /// Constuctor.
    /// @param module Owner module.
    explicit EC_MenuContainer(IModule *module);
    EC_Placeable *cameraPlaceable;
    Vector3df ownEntityPos;
    Vector3df distance;
    QPointer<QListView> listview_;

    Foundation::RenderServiceInterface *renderer_;

    QList<EC_MenuItem*> MenuItemList_;

    MenuDataModel *menudatamodel_;

    QPoint mousePosition;

    bool menuClicked_;
    bool subMenu_clicked_;
    bool subMenu_;
    bool subMenuIsScrolling;
    bool startingPositionSaved_;

    float speed_;
    float radius_;
    float subMenuRadius_;
    InputContextPtr input_;
    int selected_;
    int previousSelected_;
    int subMenuItemSelected_;
    int menulevels_;

    /// Internal timer for kinetic scroller.
    QTimer *scrollerTimer_;
    int scrollerTimer_Interval;

public:
    /// Destructor.
    ~EC_MenuContainer();

    //! Specifies whether menu is following a camera or not.
    Q_PROPERTY(bool follow READ getfollow WRITE setfollow);
    DEFINE_QPROPERTY_ATTRIBUTE(bool, follow);

public slots:

    /// Handles kinetic scrolling for both, menu and submenu items.
    void KineticScroller();

    /// Handle MouseEvents
    void HandleMouseInputEvent(MouseEvent *mouse);

    /// Sets data for MenuContainer.
    /// \param QList of QWidgets, each widget should have one layout with widgets for submenu.
    void SetMenuWidgets(QList<QWidget*>);

    /// Add component to menu
    /// \param QString reference for mesh to use.
    /// \param QStringList materialreferences for that mesh.
    void AddComponentToMenu(QString meshref, QStringList materialList, int itemnumber=0);

    void ActivateMenu();

    /// Prepares MenuContainer component
    /// \param float menu radius
    void PrepareMenuContainer(float radius, MenuDataModel *parent=0);

    void OpenMenu();

    QObject* GetMenuDataModel();


private slots:

    /// Monitors this entitys added components.
    void ComponentAdded(IComponent *component, AttributeChange::Type change);

    /// Monitors this entitys removed components.
    void ComponentRemoved(IComponent *component, AttributeChange::Type change);

    //! Handle attributechange
    /*! \param attribute Attribute that changed.
        \param change Change type.
     */
    void AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType);

    /// Create placeable component or if there allready is one in parent entity returns pointer to that one.
    EC_Placeable *GetOrCreatePlaceableComponent();

    /// Centers menuitems after rotation, so that selectet item is always nearest one.
    void CenterAfterRotation();

    /// Sets MenuContainer's position in front of camera.
    void SetMenuContainerPosition();

    EC_MenuItem* CreateMenuItem();
    EC_MenuItem* CreateMenuItem(ComponentPtr parentPlaceable);


signals:
    void OnAttributeChanged(IAttribute*, AttributeChange::Type);

    /// This signal is emitted when one of the menuitems was selected.
    /// \param menuitem indicates which item was selected from main layer
    /// \param submenuItem indicates which item was selected from sublayer.
    void OnMenuSelection(int menuitem, int submenuItem);

};

#endif
