import Qt 4.7

Rectangle {
    id: mainWindow
    width:  1000
    height: 600

    signal stopRendering();
    signal startRendering();

    Rectangle {
        id: sidePanel
        width: 200
        color: "lightblue"
        anchors.left: mainWindow.left
        anchors.top: mainWindow.top
        anchors.bottom: mainWindow.bottom
        clip: false
        ListView {
            anchors.fill: parent
            model: qmlList
            delegate: listDelegate
        }

        Text {
            anchors.bottom: txtStart.top
            id: txtStop
            font.pointSize: 18;
            text: "Stop Rendering"
            MouseArea {
                anchors.fill: parent
                onClicked: stopRendering()
            }
        }

        Text {
            anchors.bottom: sidePanel.bottom
            id: txtStart
            font.pointSize: 18;
            text: "Start Rendering"
            MouseArea {
                anchors.fill: parent
                onClicked: startRendering()
            }
        }
    }

    Rectangle {
        color: "lightgrey"
        anchors { left: sidePanel.right; top: mainWindow.top; bottom: mainWindow.bottom; right: mainWindow.right }
        clip: true
        Loader {
            anchors.fill: parent
            id: loaderqml
            source:  ""
        }
    }

    Component {
        id: listDelegate
        Column {
            Text {
                text: name; font.pointSize: 20;
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        loaderqml.source = source
                    }
                }
            }
        }
    }

    ListModel {
        id: qmlList

         ListElement {
             name: "PhotoViewer"
             source: "./photoviewer/photoviewer.qml"
         }
         ListElement {
             name: "Samegame"
             source: "./samegame/samegame.qml"
         }
         ListElement {
             name: "Snake"
             source: "./snake/snake.qml"
         }
         ListElement {
             name: "Calculator"
             source: "./calculator/calculator.qml"
         }
         ListElement {
             name: "RSS news"
             source: "./rssnews/rssnews.qml"
         }
         ListElement {
             name: "Flickr"
             source: "./flickr/flickr.qml"
         }

     }
}
