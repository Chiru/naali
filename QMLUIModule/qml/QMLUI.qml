import QtQuick 1.0

Rectangle {
    id: uiRect
    color: "transparent"
    opacity: 1
    signal exit
    property int usingbat: 0
    //function xmlfunction(xmlsource) { xmlModel.source = xmlsource }
    function networkstatechanged(networkstate) {
        if (networkstate == 0)
            networkText.text = "Network state: Invalid"
        else if (networkstate == 1)
            networkText.text = "Network state: Not Available"
        else if (networkstate == 2)
            networkText.text = "Network state: Connecting..."
        else if (networkstate == 3)
            networkText.text = "Network state: Connected"
        else if (networkstate == 4)
            networkText.text = "Network state: Closing..."
        else if (networkstate == 5)
            networkText.text = "Network state: Disconnected"
        else if (networkstate == 6)
            networkText.text = "Network state: Roaming..."
        else
            networkText.text = "Network state: Invalid"
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
    }

    function usingbattery(usbat) {
        uiRect.usingbat = usbat
        if (usbat == 0)
        {
            pb.ss = "notusingbattery"
        }
        else
            if (pb.value < 21)
                pb.ss = "critical"
            else
                pb.ss = ""

    }

    QMLMenu {
        owner: parent
    }

    Loader { id: loader1;  }

    QMLKeyboard {
        owner: parent
    }

    QMLKeyboard2 {
        owner: uiRect
    }


    /*CIELogo {
        owner: parent
    }*/

    Text {
        id: networkText
        text: "Network state: Unknown"
        anchors.horizontalCenter: uiRect.horizontalCenter
        anchors.top:  uiRect.top
        anchors.topMargin: 30
    }

    Rectangle {
        id: batteryRect
        width:  150
        height: 25
        anchors { top: uiRect.top; topMargin: 30; right: uiRect.right; rightMargin: 10; }
        opacity: 0.5
        color: "black"

        MouseArea {
            id: testarea2
            anchors.fill: parent
            onClicked: loader1.source = "http://www.students.oamk.fi/~t8kool00/test.qml";
        }

    }

    Rectangle {
        id: batteryTip
        width:  5
        height: 19
        anchors { right: batteryRect.left; verticalCenter: batteryRect.verticalCenter }
        opacity: 0.5
        color: "black"

    }



        ProgressBar {
            id: pb
            owner: parent
    }

        PathExample {

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
        model:  xmlModel
        delegate: Text { text: "EntityID:" + entityID; MouseArea { anchors.fill: parent } }
    }*/


}

