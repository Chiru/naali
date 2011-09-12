import Qt 4.7
Item {
    property alias owner: openMenu.parent
    signal onSetVisible
    Image {
        id: openMenu
        source: "./images/arrow.png"
        anchors.left: owner.left
        y: 60

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
                target: openMenu; y: 230
            }
        }

        transitions: Transition {
            from: ""
            to: "open"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y"; duration: 500; }
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
    }



    Text {
        id: oneMenu
        text: "View"
        y: -100
        x: 10
        opacity: 0
        scale: 1
        font.pointSize: 24;

        states: State {
            name: "down"
            PropertyChanges {
                target: oneMenu; y: 80; opacity: 1; scale: 1
            }
        }

        transitions: Transition {
            from: ""
            to: "down"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y, opacity, scale"; duration: 500; }
            }
            }

        MouseArea {
            id: oneMenuMA
            anchors.fill: parent
            onClicked: {
                if (keyboard.state == "")
                {
                    keyboard.state = "open"
                    timer.state = ""
                }
                else {
                    keyboard.state = ""
                }

            }
        }

        Rectangle {
            id: keyboard
            width:  keyboard1.width + 10
            height: keyboard1.height + 10
            anchors.verticalCenter: parent.bottom
            opacity: 0
            border.width: 1
            border.color: "black"
            color: "lightblue"
            radius: 15

            Text {
                id: keyboard1
                text: "Keyboard"
                anchors.centerIn: parent
                opacity: 1
                font.pointSize: 18;

                MouseArea {
                    anchors.fill: parent
                    onClicked: { setVisible(); settedit() }
                }
            }

            Rectangle {
                id: circlemenu
                width:  circlemenu1.width + 10
                height: circlemenu1.height + 10
                anchors.left: keyboard.right
                anchors.leftMargin: 5
                border.width: 1
                border.color: "black"
                color: "lightblue"
                radius: 15

                Text {
                    id: circlemenu1
                    text: "Circle menu"
                    anchors.centerIn: parent
                    opacity: 1
                    font.pointSize: 18;

                    MouseArea {
                        anchors.fill: parent
                        onClicked: setPathview()
                    }
                }
            }

            Rectangle {
                id: logo
                width:  logo1.width + 10
                height: logo1.height + 10
                anchors.left: circlemenu.right
                anchors.leftMargin: 5
                border.width: 1
                border.color: "black"
                color: "lightblue"
                radius: 15

                Text {
                    id: logo1
                    text: "Logo"
                    anchors.centerIn: parent
                    opacity: 1
                    font.pointSize: 18;

                    MouseArea {
                        anchors.fill: parent
                        onClicked: setlogo()
                    }
                }
            }

            states: State {
                name: "open"
                PropertyChanges {
                    target: keyboard; x: 130; opacity: 1
                }
            }

            transitions: Transition {
                from: ""
                to: "open"
                reversible: true
                ParallelAnimation {
                    NumberAnimation { properties: "x, opacity"; duration: 300; }
                }
            }
        }

    }

    Text {
        id: twoMenu
        text: "Load"
        y: -100
        x: 10
        opacity: 0
        font.pointSize: 24;

        states: State {
            name: "down"
            PropertyChanges {
                target: twoMenu; y: 130; opacity: 1
            }
        }

        transitions: Transition {
            from: ""
            to: "down"
            reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "y, opacity"; duration: 500; }
            }
        }

        MouseArea {
            id: twoMenuMA
            anchors.fill: parent
            onClicked: {
                if (timer.state == "")
                {
                    keyboard.state = ""
                    timer.state = "open"
                }
                else {
                    timer.state = ""
                }

            }
        }


        Rectangle {
            id: timer
            width: timer1.width + 10
            height: timer1.height + 10
            color: "lightblue"
            border.width: 1
            border.color: "black"
            radius: 15
            anchors.verticalCenter: parent.bottom
            opacity: 0


            Text {
                id: timer1
                text: "Timer"
                anchors.centerIn: parent
                opacity: 1
                font.pointSize: 18;

                    MouseArea {
                        id: submenuMA2
                        anchors.fill: parent
                        onClicked: loadtimer()
                    }
                }

                states: State {
                    name: "open"
                    PropertyChanges {
                        target: timer; x: 130; opacity: 1
                    }
                }

                transitions: Transition {
                    from: ""
                    to: "open"
                    reversible: true
                    ParallelAnimation {
                        NumberAnimation { properties: "x, opacity"; duration: 300; }
                    }
                }

        }

    }

    Text {
        id: threeMenu
        text: "Exit"
        y: -100
        x: 10
        opacity: 0
        font.pointSize: 24;

        states: State {
            name: "down"
            PropertyChanges {
                target: threeMenu; y: 180; opacity: 1
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
                exit()
            }
        }
    }
}
