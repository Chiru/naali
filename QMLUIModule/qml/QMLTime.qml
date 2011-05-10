import Qt 4.7

Item {

    width: textTime.width
    height: textTime.height

    Rectangle {
        id: rectTime
        width:  textTime.width
        height: textTime.height
        color: "white"

        Timer {
            id: timer
            interval: 10; running: true; repeat: true
            onTriggered: { textTime.text = Qt.formatDateTime(new Date(), "hh:mm:ss:zzz")
            }
        }

        Text {
            id: textTime
            anchors.centerIn: parent
        }
    }
}
