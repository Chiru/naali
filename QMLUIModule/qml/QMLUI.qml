import Qt 4.7

Rectangle {
    id: uiRect
    color: "transparent"
    opacity: 1
    signal exit
    signal loadxml
    signal setFocus(bool focus)
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
            networkText.visible = false
            networkmodechanged.visible = false }
        else {
            networkText.visible = true
            networkMode.visible = true }
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
    function xmlfunction(xmlsource) { xmlModel.source = xmlsource }
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


    MouseArea {
        anchors.fill: parent

        onClicked: { setFocus(false);}
    }

    Loader {
        id: loaderTimer
        source: ""
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 30
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
        anchors.horizontalCenter: uiRect.horizontalCenter
        anchors.top:  uiRect.top
        anchors.topMargin: 30
        visible: false
    }
    Text {
        id: networkMode
        text: "Network mode: Unknown"
        anchors.horizontalCenter: networkText.horizontalCenter
        anchors.top: networkText.bottom
        anchors.topMargin: 5
        visible: false
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
        }

        GridViewKeyboard {
            id: gdKeyboard
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 50
            visible: false
        }

    /*XmlListModel {
         id: xmlModel
         source: ""
         query: "/scene/entity"

         XmlRole { name: "entityID"; query: "@id/string()" }
     }

    ListView {
        width:  200
        height: 600
        anchors.top: uiRect.top
        anchors.topMargin: 30
        anchors.right: uiRect.right
        anchors.rightMargin: 100
        model:  xmlModel
        delegate: Text { text: "EntityID:" + entityID; MouseArea { anchors.fill: parent } }
    }*/




}

