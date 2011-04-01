import QtQuick 1.0

Item {

    Rectangle {
            id: pathviewRect
            x: 30
            y: 300
            width: 280
            height: 160
            color: "transparent"





        Component {
                 id: delegate

                 Item {
                     id: wrapper
                     width: 280; height: 160
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
                     startX: 250 ; startY: 120
                     PathAttribute { name: "iconScale"; value: 1.0 }
                     PathAttribute { name: "iconOpacity"; value: 1.0 }
                     PathQuad { x: 250; y: 45; controlX: 390; controlY: 95 }
                     PathAttribute { name: "iconScale"; value: 0.3 }
                     PathAttribute { name: "iconOpacity"; value: 0.3 }
                     PathQuad { x: 250; y: 120; controlX: -20; controlY: 95 }
                 }
             }
        }
}
