/**
 *  For conditions of distribution and use, see copyright notice in license.txt
 *
 *  @file   EC_Template.h
 *  @brief  EC_Template is empty template for EC components.
 *          This componen can be used as a template when creating new EC components.
 *  @note   no notes
 *
 */

#ifndef incl_EC_Menu_EC_Menu_h
#define incl_EC_Menu_EC_Menu_h

#include "IComponent.h"
#include "IAttribute.h"
#include "Declare_EC.h"


#include "SceneFwd.h"
#include "InputFwd.h"

#include <QString>
#include <QSize>
#include <QPointer>
#include <QPoint>
#include <QTimer>
#include <QMenu>
#include <QList>
#include <QPointer>

#include "EC_3DCanvas.h"
#include "EC_Mesh.h"
#include "EC_Placeable.h"
//#include <QtDeclarative/QtDeclarative>

/**
<table class="header">
<tr>
<td>
<h2>Template</h2>
Empty template for EC components.
This componen can be used as a template when creating new EC components.

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
class EC_OgreCamera;
class QListView;
class QMouseEvent;
class RaycastResult;

class EC_Menu : public IComponent
{
    Q_OBJECT
    DECLARE_EC(EC_Menu);

private:
    /// Constuctor.
    /// @param module Owner module.
    explicit EC_Menu(IModule *module);
    EC_Placeable *cameraPlaceable;
    Vector3df ownEntityPos;
    Vector3df distance;
    QPointer<QListView> listview_;

    Foundation::RenderServiceInterface *renderer_;

    QList<EC_Mesh *> MeshList_;
    bool ent_clicked_;
    bool save_start_position_;
    QPoint startPosition_;

    QPoint acceleratorVector_;
    InputContextPtr input_;

    //QDeclarativeView *view_;
    void SetEntityPosition();

    void kinecticScroller(QPoint);

    //! Internal timer for updating inworld EC_3DCanvas.
    QTimer *renderTimer_;

public:
    /// Destructor.
    ~EC_Menu();

    //! Rendering target submesh index.
    Q_PROPERTY(int renderSubmeshIndex READ getrenderSubmeshIndex WRITE setrenderSubmeshIndex);
    DEFINE_QPROPERTY_ATTRIBUTE(int, renderSubmeshIndex);

    //! Boolean for interactive mode, if true it will show context menus on mouse click events.
    Q_PROPERTY(bool interactive READ getinteractive WRITE setinteractive);
    DEFINE_QPROPERTY_ATTRIBUTE(bool, interactive);


public slots:
    void Render();

    //! Handle MouseEvents
    void HandleMouseInputEvent(MouseEvent *mouse);


private slots:
    //! Prepares everything related to the parent widget and other needed components.
    void PrepareMenu();

    //! Monitors this entitys added components.
    void ComponentAdded(IComponent *component, AttributeChange::Type change);

    //! Monitors this entitys removed components.
    void ComponentRemoved(IComponent *component, AttributeChange::Type change);

    //! Monitors this components Attribute changes.
    void AttributeChanged(IAttribute *attribute, AttributeChange::Type changeType);

    //! Create as many EC_Mesh components to the parent entity as given in input. Returns empty QList if parent entity is not present
    QList<EC_Mesh*> CreateMeshComponents(int);

    //! Get parent entitys EC_3DCanvas. Return 0 if not present.
    EC_3DCanvas *GetOrCreateSceneCanvasComponent();

    EC_Placeable *GetOrCreatePlaceableComponent();

    //! Handles entity action WebViewControllerChanged
    /// \note The action signature is (string)"WebViewControllerChanged", (int)"id", (string)"name"
    void ActionControllerChanged(QString id, QString newController);

signals:
    void OnAttributeChanged(IAttribute*, AttributeChange::Type);

};

#endif
