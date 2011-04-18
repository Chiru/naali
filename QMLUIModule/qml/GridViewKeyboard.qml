import Qt 4.7

Rectangle {
    width: 660; height: 180
    color: "transparent"

    Component {
        id: keyboardDelegate
        Item {
            width: grid.cellWidth; height: grid.cellHeight
            Column {
                id: column
                anchors.fill: parent


                Rectangle {
                    id: buttonShadow;
                    color: "black";
                    opacity: 0.6;
                    width: buttonRect.width
                    height: buttonRect.height
                    anchors.verticalCenter: buttonRect.verticalCenter
                    anchors.horizontalCenter: buttonRect.horizontalCenter
                    radius: buttonRect.radius
                    anchors.verticalCenterOffset: 3
                    anchors.horizontalCenterOffset: 3
                }
                Rectangle {
                    id: buttonRect;
                    anchors.top: parent.top;
                    anchors.left: parent.left;
                    border.color: "black"; border.width: 1;
                    width:  grid.cellWidth - 5;
                    height: grid.cellHeight - 5
                    radius: 10

                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            parent.state = "down"
                            getChar(name)
                        }
                        onReleased: parent.state = ""
                    }

                    states: State {
                        name:  "down"
                        PropertyChanges {
                            target:  buttonRect
                            anchors.topMargin: 3
                            anchors.leftMargin: 3
                            color: "lightblue"
                        }
                        PropertyChanges {
                            target:  buttonShadow
                            anchors.verticalCenterOffset: 0
                            anchors.horizontalCenterOffset: 0
                        }
                    }

                    transitions: [ Transition {
                        from: ""; to: "down"; //reversible: true;
                        NumberAnimation {
                            //loops: 1
                            properties: "topMargin, leftMargin"
                            from: 0
                            to: 3
                            duration: 100
                            onCompleted: buttonRect.state = ""


                        }
                    },
                        Transition {
                            from: "down"; to: ""; //reversible: true;
                            NumberAnimation {
                                //loops: 1
                                properties: "topMargin, leftMargin"
                                from: 3
                                to: 0
                                duration: 100



                            } }


                    ]
                }
                Text { text: name; anchors.centerIn: buttonRect }
            }
        }
    }

    GridView {
        interactive: false
        id: grid
        anchors.fill: parent
        cellWidth: 60; cellHeight: 60

        model: KeyboardModel {}
        delegate: keyboardDelegate
        /*highlight: Rectangle {
            color: "lightsteelblue";
            radius: 10;
            anchors.left: grid.left; anchors.top: grid.top
            width: parent.cellWidth;
            height: parent.cellHeight } */
        focus: true
    }
}
