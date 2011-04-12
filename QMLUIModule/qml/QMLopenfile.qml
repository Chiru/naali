import Qt 4.7

Item {

    FocusScope {
        id: focusScope
        width: borderRect.width
        height: borderRect.height
        x: 100
        y: 100

        Rectangle {
            id: borderRect
            property int widthDest: 20 + textEdit.width
            width: widthDest

            height: 30
            color: "gray"
            anchors.centerIn: parent
            radius: 10
            border.color: "black"
            border.width: 2






            Rectangle {
                id: textRect
                color: white
                width:  borderRect.width - 4
                height: borderRect.height - 4
                anchors.centerIn: parent
                radius: 10

                TextInput {
                     id: textEdit
                     text: ""
                     cursorVisible: false
                     focus: true
                     anchors.centerIn: parent
                 }
             }

            MouseArea {
                anchors.fill: parent
                drag.target: focusScope; drag.axis: Drag.XandYAxis
                onClicked:  {
                    setFocus(true); focusScope.focus = true; textEdit.selectAll()
                    if (textEdit.focus)
                    {
                        textRect.color = "lightgreen"
                        textEdit.cursorVisible = true
                        textEdit.openSoftwareInputPanel();
                    }
                    else
                    {
                        textRect.color = "white"
                        textEdit.cursorVisible = false
                    }
                }
            }
     }
    }
}
