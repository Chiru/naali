import Qt 4.7

Item {

    property alias owner: kbimg.parent
    property alias visiblee: kbimg.visible

Image {
    id: kbimg
    source: "./images/keyboard.png"
    anchors.horizontalCenter: owner.horizontalCenter
    y: owner.height - 30
    scale: 1
    opacity: 1


    MouseArea {
        id: kbma
        anchors.fill: parent
        onClicked: {
            if (kbimg.state == "")
                kbimg.state = "down"
            else
                kbimg.state = ""
        }
    }

    states: State {
        name: "down"
        PropertyChanges {
            target: kbimg; y: owner.height - kbma.height + 20 ; opacity: 0.6
        }
    }

    transitions: Transition {
        from: ""
        to: "down"
        reversible: true
        ParallelAnimation {
            NumberAnimation {
                properties: "y, opacity"; duration: 500
            }
        }
    }

}

}
