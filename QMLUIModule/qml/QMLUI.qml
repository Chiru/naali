import Qt 4.7

Rectangle {
    id: uiRect
    color: "transparent"
    opacity: 1
    width: screenwidth
    height: screenheight
    signal exit
    signal pinching(int i)
    signal qmlmoving(int i)
    signal move(string direction)

    //signal setVisible
    property int usingbat: 0

    function networkmodechanged(mode) {
        if (mode  == 0)
            networkMode.text = "Network mode: Unknown"
        else if (mode == 1)
            networkMode.text = "Network mode: Gsm"
        else if (mode == 2)
            networkMode.text = "Network mode: Cdma"
        else if (mode == 3)
            networkMode.text = "Network mode: Wcdma"
        else if (mode == 4)
            networkMode.text = "Network mode: Wlan"
        else if (mode == 5)
            networkMode.text = "Network mode: Ethernet"
        else if (mode == 6)
            networkMode.text = "Network mode: Bluetooth"
        else if (mode == 7)
            networkMode.text = "Network mode: Wimax"
        else
            networkMode.text = "Network mode: Invalid"

    }


    function setVisible() {
        if (gdKeyboard.visible == true)
            gdKeyboard.visible = false
        else
            gdKeyboard.visible = true
    }
    function setKeyboard1() {
        if (keyboard1.visiblee == true)
            keyboard1.visiblee = false
        else
            keyboard1.visiblee = true
    }
    function setKeyboard2() {
        if (keyboard2.visiblee == true)
            keyboard2.visiblee = false
        else
            keyboard2.visiblee = true
    }
    function setPathview() {
        if (pathexample.visible == true)
            pathexample.visible = false
        else
            pathexample.visible = true
    }
    function setnetworktext() {
        if (networkText.visible == true) {
            networkText.visible = false;
            networkMode.visible = networkText.visible }
        else {
            networkText.visible = true;
            networkMode.visible = networkText.visible }
    }
    function setpb() {
        if (pb.visiblee == true)
            pb.visiblee = false
        else
            pb.visiblee = true
    }
    function settedit() {
        if (textEdit.visible == true)
            textEdit.visible = false
        else
            textEdit.visible = true
    }
    function setlogo() {
        if (cielogo.visible == true)
            cielogo.visible = false
        else
            cielogo.visible = true
    }
    function networkstatechanged(networkstate) {
        if (networkstate == 0)
            networkText.text = "Network state: Undetermined"
        else if (networkstate == 1)
            networkText.text = "Network state: Connecting"
        else if (networkstate == 2)
            networkText.text = "Network state: Connected"
        else if (networkstate == 3)
            networkText.text = "Network state: Disconnected"
        else if (networkstate == 4)
            networkText.text = "Network state: Roaming"
        else
            networkText.text = "Network state: Undetermined"
    }

    function batterylevelchanged(batterylevel) {
        pb.value = batterylevel
        if (uiRect.usingbat == 1)
        {
            if (batterylevel < 21)
                pb.ss = "critical"
            else
                 pb.ss = ""
        }
        else
        {
            if (batterylevel == 100 && usingbat == 0)
            {
                pb.ss = "batteryfull"
            }
            else if (batterylevel < 100 && usingbat == 0)
                pb.ss = "notusingbattery"
        }
    }

    function usingbattery(usbat) {
        uiRect.usingbat = usbat
        if (usbat == 0 && pb.value < 100)
        {
            pb.ss = "notusingbattery"
        }
        else if (usbat == 0 && pb.value == 100)
            pb.ss = "batteryfull"
        else
            if (pb.value < 21)
                pb.ss = "critical"
            else
                pb.ss = ""

    }

    function getChar(text) {
        if (text == "+")
            textEdit.edtext = textEdit.edtext + " "
        else if (text == "-") {
            textEdit.edtext = "" }
        else
            textEdit.edtext = textEdit.edtext + text
    }

    function loadtimer() {
        if (loaderTimer.source == "")
            loaderTimer.source = "./QMLTime.qml"
        else
            loaderTimer.source = ""
    }


    /*MouseArea {
        anchors.fill: parent
    }*/

    Loader {
        id: loaderTimer
        source: ""
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 130
        anchors.rightMargin: 10
    }

    QMLMenu {
        id: qmlmenu
        owner: parent
    }

    Loader { id: loader1;  }

    QMLKeyboard {
        id: keyboard1
        owner: parent
        visiblee: false

    }

    QMLKeyboard2 {
        id: keyboard2
        owner: uiRect
        visiblee: false
    }


    CIELogo {
        id: cielogo
        visible: false
    }

    Text {
        id: networkText
        text: "Network state: Unknown"
        anchors.left: uiRect.left
        anchors.bottom:  uiRect.bottom
        visible: false

        Text {
            id: networkMode
            text: "Network mode: Unknown"
            anchors.left: networkText.right
            anchors.top: networkText.top
            anchors.leftMargin: 5
            visible: networkText.visible
        }
    }


        ProgressBar {
            id: pb
            owner: parent
            visiblee: false
    }

        PathExample {
            id: pathexample
            visible: false

        }

        QMLopenfile {
            id: textEdit
            visible: false
            //anchors.horizontalCenter: gdKeyboard.horizontalCenter
            //anchors.bottom: gdKeyboard.top
            //anchors.bottomMargin: 5

        }

        GridViewKeyboard {
            id: gdKeyboard
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            visible: false
        }

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 5
            color: "white"
            width: 120
            height: 60
            border.width: 1
            border.color: "black"
            radius: 15
            Text {
                id: pinchtext
                text: "Pinching OFF"
                color: "black"
                anchors.centerIn: parent
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (pinchtext.text == "Pinching OFF")
                    {
                        pinchtext.text = "Pinching ON"
                        pinching(1)
                    }
                    else
                    {
                        pinchtext.text = "Pinching OFF"
                        pinching(0)
                    }
                }
            }
        }
        Rectangle {
            id: centerball
            color: "red"
            width:  20
            height: 20
            radius: 360
            opacity: pinchopacity
            x: pinchcenterx - centerball.width / 2
            y: pinchcentery - centerball.height / 2
        }
        Rectangle {
            id: ball1
            color: "red"
            width:  30
            height: 30
            radius: 360
            opacity: pinchopacity
            x: pinchx - ball1.width / 2
            y: pinchy - ball1.height / 2
        }
        Rectangle {
            id: ball2
            color: "red"
            width:  30
            height: 30
            radius: 360
            opacity: pinchopacity
            x: pinchxx - ball2.width / 2
            y: pinchyy - ball2.height / 2
        }

        Grid {
            rows: 3
            columns: 3
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5

            Rectangle {
                id: upleft
                color: "transparent"
                opacity: 0.5
                width: 75
                height: 75
            }

            Rectangle {
                id: up
                color: "gray"
                opacity: 0.5
                width: 75
                height: 75
                border.width: 1
                border.color: "black"
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        up.color = "green"
                        move("up")
                    }
                    onReleased: {
                        up.color = "gray"
                        move("stopup")
                    }
                }
            }

            Rectangle {
                id: upright
                color: "transparent"
                opacity: 0.5
                width: 75
                height: 75
            }

            Rectangle {
                id: left
                color: "gray"
                opacity: 0.5
                width: 75
                height: 75
                border.width: 1
                border.color: "black"
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        left.color = "green"
                        move("left")
                    }
                    onReleased: {
                        left.color = "gray"
                        move("stopleft")
                    }
                }
            }
            Rectangle {
                id: middle
                color: "gray"
                opacity: 0.5
                width: 75
                height: 75
                border.width: 1
                border.color: "black"
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        middle.color = "green"
                        move("stop")
                    }
                    onReleased: {
                        middle.color = "gray"
                    }
                }
            }

            Rectangle {
                id: right
                color: "gray"
                opacity: 0.5
                width: 75
                height: 75
                border.width: 1
                border.color: "black"
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        right.color = "green"
                        move("right")
                    }
                    onReleased: {
                        right.color = "gray"
                        move("stopright")
                    }
                }
            }

            Rectangle {
                id: downleft
                color: "transparent"
                opacity: 0.5
                width: 75
                height: 75
            }

            Rectangle {
                id: down
                color: "gray"
                opacity: 0.5
                width: 75
                height: 75
                border.width: 1
                border.color: "black"
                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        down.color = "green"
                        move("down")
                    }
                    onReleased: {
                        down.color = "gray"
                        move("stopdown")
                    }
                }
            }

            Rectangle {
                id: downright
                color: "transparent"
                opacity: 0.5
                width: 75
                height: 75
            }
        }
}

