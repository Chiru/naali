import QtQuick 1.0
Item {
    property alias owner: openMenu.parent
    Text {
        id: openMenu
        text: "Open Menu"
        anchors.left: owner.left
        y: 30
        font.pointSize: 24; font.bold: true
        opacity: 1

        MouseArea {
            id: openMouseArea
            anchors.fill: parent
            onClicked: {
                if (openMenu.state == "")
                {
                    openMenu.state = "open"
                    oneMenu.state = "down"
                    twoMenu.state = "down"
                    threeMenu.state = "down"
                }
                else
                {
                    openMenu.state = ""
                    oneMenu.state = ""
                    twoMenu.state = ""
                    threeMenu.state = ""
                }
            }
        }

        states: State {
            name: "open"
            PropertyChanges {
                target: openMenu; y: 180; text: "Close Menu"
            }
        }

        transitions: Transition {
            from: ""
            to: "open"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y"; duration: 500; /*easing.type: Easing.OutInBounce*/ }
            }
        }
    }



    Text {
        id: oneMenu
        text: "oneMenu"
        //anchors.left: owner.left
        y: -100
        opacity: 0
        scale: 1
        font.pointSize: 24; font.bold: true

        states: State {
            name: "down"
            PropertyChanges {
                target: oneMenu; y: 30; opacity: 1; scale: 1
            }
        }

        transitions: Transition {
            from: ""
            to: "down"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y, opacity, scale"; duration: 500; /*easing.type: Easing.OutInBounce*/ }
            }
            }

        MouseArea {
            id: oneMenuMA
            anchors.fill: parent
            onClicked: {
                if (submenu1.state == "")
                {
                    submenu1.state = "open"
                    submenu2.state = ""
                    submenu3.state = ""
                }
                else {
                    submenu1.state = ""
                }

            }
        }

        Text {
            id: submenu1
            text: "submenu1"
            anchors.verticalCenter: parent.bottom
            opacity: 0
            font.pointSize: 18; font.bold: true

            states: State {
                name: "open"
                PropertyChanges {
                    target: submenu1; x: 130; opacity: 1
                }
            }

            transitions: Transition {
                from: ""
                to: "open"
                reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "x, opacity"; duration: 300; /*easing.type: Easing.InQuad*/ }
                }
            }
        }

    }

    Text {
        id: twoMenu
        text: "twoMenu"
        //anchors.left: owner.left
        y: -100
        opacity: 0
        font.pointSize: 24; font.bold: true

        states: State {
            name: "down"
            PropertyChanges {
                target: twoMenu; y: 80; opacity: 1
            }
        }

        transitions: Transition {
            from: ""
            to: "down"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y, opacity"; duration: 500; /*easing.type: Easing.OutInBounce*/ }
            }
        }

        MouseArea {
            id: twoMenuMA
            anchors.fill: parent
            onClicked: {
                if (submenu2.state == "")
                {
                    submenu1.state = ""
                    submenu2.state = "open"
                    submenu3.state = ""
                }
                else {
                    submenu2.state = ""
                }

            }
        }

        Text {
            id: submenu2
            text: "submenu2"
            anchors.verticalCenter: parent.bottom
            opacity: 0
            font.pointSize: 18; font.bold: true

            states: State {
                name: "open"
                PropertyChanges {
                    target: submenu2; x: 130; opacity: 1
                }
            }

            transitions: Transition {
                from: ""
                to: "open"
                reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "x, opacity"; duration: 300; /*easing.type: Easing.OutInBounce*/ }
                }
            }
        }

    }

    Text {
        id: threeMenu
        text: "threeMenu"
        //anchors.left: owner.left
        y: -100
        opacity: 0
        font.pointSize: 24; font.bold: true

        states: State {
            name: "down"
            PropertyChanges {
                target: threeMenu; y: 130; opacity: 1
            }
        }

        transitions: Transition {
            from: ""
            to: "down"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y, opacity"; duration: 500; /*easing.type: Easing.OutInBounce*/ }
            }
        }

        MouseArea {
            id: threeMenuMA
            anchors.fill: parent
            onClicked: {
                if (submenu3.state == "")
                {
                    submenu1.state = ""
                    submenu2.state = ""
                    submenu3.state = "open"
                }
                else {
                    submenu3.state = ""
                }

            }
        }

        Text {
            id: submenu3
            text: "Exit"
            anchors.verticalCenter: parent.bottom
            opacity: 0
            font.pointSize: 18; font.bold: true

            states: State {
                name: "open"
                PropertyChanges {
                    target: submenu3; x: 200; opacity: 1
                }
            }

            transitions: Transition {
                from: ""
                to: "open"
                reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "x, opacity"; duration: 300; /*easing.type: Easing.OutInBounce*/ }
                }
            }

            MouseArea {
                id: submenuMA
                anchors.fill: parent
                onClicked: exit()
            }
        }
    }
}
