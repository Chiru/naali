import QtQuick 1.0
import Qt3D 1.0

Rectangle {
    id: uiRect
    color: "transparent"
    opacity: 1
    signal exit
    function xmlfunction(xmlsource) { xmlModel.source = xmlsource }

    QMLMenu {
        owner: parent
    }

    QMLKeyboard {
        owner: parent
    }

    QMLKeyboard2 {
        owner: parent
    }

    /*CIELogo {
        owner: parent
    }*/



    XmlListModel {
         id: xmlModel
         source: ""
         query: "/scene/entity"

         XmlRole { name: "entityID"; query: "@id/string()" }
     }

    ListView {
        width:  200
        height: 600
        anchors.top: uiRect.top
        anchors.topMargin: 30
        anchors.right: uiRect.right
        model:  xmlModel
        delegate: Text { text: "EntityID:" + entityID; MouseArea { anchors.fill: parent } }
    }
    Rectangle {
        id: trolol
        x: 400
        y: 400
        width: 400
        height: 400
        color: "transparent"




    Component {
             id: delegate

             Item {
                 id: wrapper
                 width: 150; height: 120
                 scale: PathView.iconScale
                 opacity: PathView.iconOpacity

                 Column {

                     Image { anchors.horizontalCenter: nameText.horizontalCenter; width: 100; height: 100; source: icon }
                     Text { id: nameText; text: name; font.pointSize: 16; color: wrapper.PathView.isCurrentItem ? "red" : "black" }
                 }
             }
              }


         PathView {
             anchors.fill: parent
             model:   ContactModel {}
             delegate: delegate
             path: Path {
                 startX: 120 ; startY: 100
                 PathAttribute { name: "iconScale"; value: 1.0 }
                 PathAttribute { name: "iconOpacity"; value: 1.0 }
                 PathQuad { x: 120; y: 25; controlX: 260; controlY: 75 }
                 PathAttribute { name: "iconScale"; value: 0.3 }
                 PathAttribute { name: "iconOpacity"; value: 0.0 }
                 PathQuad { x: 120; y: 100; controlX: -20; controlY: 75 }
             }
         }
    }
}

