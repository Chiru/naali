import Qt 4.7
Item {
    property alias owner: openMenu.parent
    signal onSetVisible
    Image {
        id: openMenu
        source: "./images/arrow.png"
        anchors.left: owner.left
        y: 10

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
                target: openMenu; y: 180
            }
        }

        transitions: Transition {
            from: ""
            to: "open"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y"; duration: 500; }
                //NumberAnimation { properties: "rotation"; duration: 500; /*easing.type: Easing.OutInBounce*/ }
            }
        }
    }

    Rectangle {
        color: "lightblue"
        opacity: 0.5
        id: rectBg
        width: openMenu.width
        property int height_: openMenu.y
        height: height_
        y: 0
        //x: 0
        border.color: "black"
        border.width: 1
        radius: 5
        anchors.horizontalCenter: openMenu.horizontalCenter

        //Behavior on height { SmoothedAnimation { duration: 500 } }

    }



    Text {
        id: oneMenu
        text: "View"
        //anchors.left: owner.left
        y: -100
        x: 10
        opacity: 0
        scale: 1
        font.pointSize: 24;

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
            text: "Keyboard"
            anchors.verticalCenter: parent.bottom
            opacity: 0
            font.pointSize: 12;

            MouseArea {
                anchors.fill: parent
                onClicked: setVisible()
            }

            Text {
                id: kb1
                text: "Keyboard1"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.right
                anchors.leftMargin: 5
                opacity: 1
                font.pointSize: 12;

                MouseArea {
                    anchors.fill: parent
                    onClicked: setKeyboard1()
                }
            }

            Text {
                id: kb2
                text: "Keyboard2"
                anchors.verticalCenter: kb1.verticalCenter
                anchors.left: kb1.right
                anchors.leftMargin: 5
                opacity: 1
                font.pointSize: 12;

                MouseArea {
                    anchors.fill: parent
                    onClicked: setKeyboard2()
                }
            }

            Text {
                id: pathview
                text: "Pathview"
                anchors.verticalCenter: kb2.verticalCenter
                anchors.left: kb2.right
                anchors.leftMargin: 5
                opacity: 1
                font.pointSize: 12;

                MouseArea {
                    anchors.fill: parent
                    onClicked: setPathview()
                }
            }

            Text {
                id: battery
                text: "Battery"
                anchors.verticalCenter: pathview.verticalCenter
                anchors.left: pathview.right
                anchors.leftMargin: 5
                opacity: 1
                font.pointSize: 12;

                MouseArea {
                    anchors.fill: parent
                    onClicked: setpb()
                }
            }

            Text {
                id: network
                text: "Network"
                anchors.verticalCenter: battery.verticalCenter
                anchors.left: battery.right
                anchors.leftMargin: 5
                opacity: 1
                font.pointSize: 12;

                MouseArea {
                    anchors.fill: parent
                    onClicked: setnetworktext()
                }
            }

            Text {
                id: logo
                text: "Logo"
                anchors.verticalCenter: network.verticalCenter
                anchors.left: network.right
                anchors.leftMargin: 5
                opacity: 1
                font.pointSize: 12;

                MouseArea {
                    anchors.fill: parent
                    onClicked: setlogo()
                }
            }

            Text {
                id: tedit
                text: "TextEdit"
                anchors.verticalCenter: logo.verticalCenter
                anchors.left: logo.right
                anchors.leftMargin: 5
                opacity: 1
                font.pointSize: 12;

                MouseArea {
                    anchors.fill: parent
                    onClicked: settedit()
                }
            }


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
        text: "Load"
        //anchors.left: owner.left
        y: -100
        x: 10
        opacity: 0
        font.pointSize: 24;

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
            text: "Timer"
            anchors.verticalCenter: parent.bottom
            opacity: 0
            font.pointSize: 18;

            MouseArea {
                id: submenuMA2
                anchors.fill: parent
                onClicked: loadtimer()
            }

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
        text: "Exit"
        //anchors.left: owner.left
        y: -100
        x: 10
        opacity: 0
        font.pointSize: 24;

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
            font.pointSize: 18;

            states: State {
                name: "open"
                PropertyChanges {
                    target: submenu3; x: 130; opacity: 1
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
