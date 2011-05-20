import Qt 4.7
import Qt.labs.folderlistmodel 1.0

Rectangle {
    id: mainWindow
    width: 400
    height:  400
    color: "transparent"

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (imagePath.currentIndex == folderModel.count - 1)
            {
                imagePath.currentIndex = 0;
            }
            else
            {
                imagePath.incrementCurrentIndex();
            }
        }
    }


    PathView {
             id: imagePath
             anchors.fill: parent
             interactive: false
             path: Path {

                 startX: mainWindow.width / 2; startY: mainWindow.height / 2
                 PathLine { x: mainWindow.width / 2; y: imagePath.count * mainWindow.height + mainWindow.height}
             }

             FolderListModel {
                 id: folderModel
                 nameFilters: ["*.png", "*.jpg"]

                 folder: "./images/"
             }

             Component {
                 id: fileDelegate
                 Column {
                     Image {
                         width: mainWindow.width; height: mainWindow.height
                         fillMode: Image.PreserveAspectFit
                         smooth: true
                         source: folderModel.folder + fileName
                     }
                 }
             }
             model: folderModel
             delegate: fileDelegate
         }

    Timer {
        id: timer
        interval: 60000; running: false; repeat: true
        onTriggered: {
            if (imagePath.currentIndex == folderModel.count - 1)
            {
                imagePath.currentIndex = 0;
            }
            else
            {
                imagePath.incrementCurrentIndex();
            }
        }
    }
}
