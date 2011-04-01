import QtQuick 1.0

Item {

    property alias owner: kbimg2.parent

    Image {
            id: kbimg2
            source: "./images/keyboard.png"
            scale: 0.1


            MouseArea {
                id: kbma2
                anchors.fill: parent
                drag.target: kbimg2; drag.axis: Drag.XandYAxis
                onClicked: {
                    if (kbimg2.state == "")
                        kbimg2.state = "down"
                    else
                        kbimg2.state = ""
                }

            }

            states: State {
                name: "down"
                AnchorChanges {
                    target: kbimg2; anchors.horizontalCenter: owner.horizontalCenter; anchors.verticalCenter: undefined; anchors.bottom: owner.bottom
                }
                PropertyChanges {
                    target: kbimg2; scale: 1; opacity: 0.7; anchors.horizontalCenterOffset: 0; anchors.bottomMargin: -20
                }
            }

            transitions: Transition {
                from: ""
                to: "down"
                reversible: true
                AnchorAnimation {
                    duration: 500
                }
                NumberAnimation {
                    properties: "scale, opacity"; duration: 500
                }
            }

        }

}
