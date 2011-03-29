import QtQuick 1.0

Item {

    property alias owner: flipable.parent

    Flipable {
         id: flipable
         width: frontimg.width
         height: frontimg.height

         anchors.top: owner.top
         anchors.topMargin: 30
         anchors.right: owner.right

         property bool flipped: false

         //front: Text { text: "Chiru"; anchors.centerIn: parent; font.pointSize: 24; font.bold: true }
         //back: Text { text: "Local"; anchors.centerIn: parent; font.pointSize: 24; font.bold: true }

         front: Image {  id: frontimg; source: "./images/cie-front.png" }
         back: Image { id: backimg; source: "./images/cie-back.png" }

         transform: Rotation {
             id: rotation
             origin.x: flipable.width/2
             origin.y: flipable.height/2
             axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
             angle: 0    // the default angle
         }

         states: State {
             name: "back"
             PropertyChanges { target: rotation; angle: 180 }
             when: flipable.flipped
         }

         transitions: Transition {
             NumberAnimation { target: rotation; property: "angle"; duration: 1000 }
         }

         MouseArea {
             anchors.fill: parent
             onClicked: flipable.flipped = !flipable.flipped
         }
     }

}
