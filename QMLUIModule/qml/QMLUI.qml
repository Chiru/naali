import QtQuick 1.0

Rectangle {
    id: uiRect
    color: "transparent"
    opacity: 1
    signal exit
    signal loadxml
    signal setFocus(bool focus)
    property int usingbat: 0
    function xmlfunction(xmlsource) { xmlModel.source = xmlsource }
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

    MouseArea {
        anchors.fill: parent

        onClicked: { setFocus(false);}
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

        ProgressBar {
            id: pb
            owner: parent
    }

        PathExample {

        }

        QMLopenfile {

        }

        Rectangle {
            id: shadowRect
            width: 50
            height: 50
            color: "black"
            opacity: 0.2
            anchors.verticalCenter: buttonRect.verticalCenter
            anchors.horizontalCenter: buttonRect.horizontalCenter

            anchors.horizontalCenterOffset: 5
            anchors.verticalCenterOffset: 5
            }

        GridViewKeyboard {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom


        }










    XmlListModel {
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
    }

        /*Rectangle {
            id: messagebox
            width: 182
            height: 82
            radius:  10
            color:  "black"
            x: 500
            y: 200

            MouseArea {
                anchors.fill: parent
                drag.target: messagebox; drag.axis: Drag.XandYAxis
            }



            Rectangle {
                id: msgbox
                width:  180
                height: 80
                radius: 10
                clip: true
                color: "white"
                anchors.centerIn: parent

                Text {
                    text: "Hello from QML!"
                    id: textHello
                    anchors.horizontalCenter: msgbox.horizontalCenter
                    anchors.verticalCenter: msgbox.verticalCenter
                    anchors.verticalCenterOffset: -10
                }

                Rectangle {
                    id: okButton
                    color:  "gray"
                    width: 40; height: 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: textHello.bottom
                    anchors.topMargin: 10

                    MouseArea {
                        anchors.fill: parent
                        onClicked: messagebox.visible = false
                    }

                    Text {
                        text: "OK"
                        anchors.centerIn: parent
                    }
                }
            }
        }*/


}

