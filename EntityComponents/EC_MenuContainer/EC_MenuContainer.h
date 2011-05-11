/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Template.h
 *  @brief  EC_Template is empty template for EC components.
 *          This componen can be used as a template when creating new EC components.
 *  @note   no notes
 *
 */

#ifndef incl_EC_MenuContainer_EC_MenuContainer_h
#define incl_EC_MenuContainer_EC_MenuContainer_h

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
//#include <QtDeclarative/QtDeclarative>

/**
<table class="header">
<tr>
<td>
<h2>EC_MenuContainer</h2>
EC_MenuContainer Component. This component creates subentities with EC_MenuItem component and is parent entity for those.
This component make sure that Menu work as a whole.

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
    //! Internal timer for updating inworld EC_3DCanvas.
    QTimer *renderTimer_;

    QList<EC_MenuItem*> MenuItemList_;    
    QList<EC_MenuItem*> SubMenuItemList_;
    //QList<float> phiList;
    bool ent_clicked_;
    bool subMenu_clicked_;
    bool subMenu_;
    bool subMenuIsScrolling;

    float speed_;
    float radius_;
    InputContextPtr input_;
    int selected_;
    int numberOfMenuelements_;

    //QDeclarativeView *view_;
    //void SetEntityPosition();

    //! Internal timer for kinetic scroller.
    QTimer *scrollerTimer_;
    int scrollerTimer_Interval;

public:
    /// Destructor.
    ~EC_MenuContainer();

    //! Boolean for interactive mode, if true it will show context menus on mouse click events.
    //Q_PROPERTY(bool interactive READ getinteractive WRITE setinteractive);
    //DEFINE_QPROPERTY_ATTRIBUTE(bool, interactive);

    //! Integer for menuelements.
    //Q_PROPERTY(int interactive READ getinteractive WRITE setinteractive);
    //DEFINE_QPROPERTY_ATTRIBUTE(int, numberOfMenuelements);


public slots:
    void Render();

    void kinecticScroller();

    //! Handle MouseEvents
    void HandleMouseInputEvent(MouseEvent *mouse);


private slots:
    //! Prepares everything related to the parent widget and other needed components.
    void PrepareMenuContainer();

    //! Monitors this entitys added components.
    void ComponentAdded(IComponent *component, AttributeChange::Type change);

    //! Monitors this entitys removed components.
    void ComponentRemoved(IComponent *component, AttributeChange::Type change);

    //! Monitors this components Attribute changes.
    void AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType);

    EC_Placeable *GetOrCreatePlaceableComponent();

    //! Handles entity action WebViewControllerChanged
    /// \note The action signature is (string)"WebViewControllerChanged", (int)"id", (string)"name"
    void ActionControllerChanged(QString id, QString newController);

    void centerAfterRotation();
    void setMenuContainerPosition();
    void createSubMenu(int);
    EC_MenuItem* CreateMenuItem();

signals:
    void OnAttributeChanged(IAttribute*, AttributeChange::Type);

};

#endif
