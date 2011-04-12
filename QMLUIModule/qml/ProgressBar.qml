import QtQuick 1.0

 Item {
     id: progressbar

     property int minimum: 0
     property int maximum: 100
     property int value: 0

     property alias owner: batteryRect.parent
     property alias ss: highlight.state
     width: 146; height: 21

     Rectangle {
         id: batteryRect
         width:  150
         height: 25
         anchors { top: owner.top; topMargin: 30; right: owner.right; rightMargin: 10; }
         opacity: 0.5
         color: "black"

         MouseArea {
             id: testarea2
             anchors.fill: parent
             onClicked:  {
                 if (loader1.source == "")
                    loader1.source = "http://www.students.oamk.fi/~t8kool00/test.qml";
                 else
                     loader1.source = ""

             }
         }

     Rectangle {
         id: highlight
         opacity: 1
         smooth: true
         clip: true
         anchors { verticalCenter: batteryRect.verticalCenter; right: batteryRect.right; rightMargin: 2 }
         property int widthDest: ((progressbar.width * (value - minimum)) / (maximum - minimum) )

         width: highlight.widthDest
         height: progressbar.height
         state:  ""
         Behavior on width {
             SmoothedAnimation { velocity: 120 }
         }

         gradient: Gradient {
             GradientStop { id: gradient1; color: "green"; position: 0.0 }
                      GradientStop { id: gradient2; color:  "white"; position: 1.0 }
         }

         states:  [ State {

                 name: ""
                 PropertyChanges { target: gradient1; color: "green" }
                 PropertyChanges { target: gradient2; color: "white" }
             },

             State {

             name: "critical"
             PropertyChanges { target: gradient1; color: "red" }
             PropertyChanges { target: gradient2; color: "white" }
         },

             State {

             name: "batteryfull"
             PropertyChanges { target: gradient1; color: "blue" }
             PropertyChanges { target: gradient2; color: "white" }
         },

             State {
             name:  "notusingbattery"

             PropertyChanges {
                target: batterylevel; text: "Charging...";  }
     }
         ]

         transitions:  [ Transition {
             from: ""; to: "critical";
                            ColorAnimation { duration: 1000 }
         },
             Transition {
                          from: "notusingbattery"; to: "";
                              ColorAnimation { duration: 1000 }
                      },
             Transition {
                          from: "notusingbattery"; to: "batteryfull";
                              ColorAnimation { duration: 1000 }
                      },
             Transition {
                          from: "batteryfull"; to: "";
                              ColorAnimation { duration: 1000 }
                      },
             Transition {
                          from: "batteryfull"; to: "critical";
                              ColorAnimation { duration: 1000 }
                      },
             Transition {
                          from: "notusingbattery"; to: "critical";
                              ColorAnimation { duration: 1000 }
                      },
             Transition {
                 from: ""; to: "notusingbattery";

                 PropertyAnimation {
                     properties: "opacity"; duration:  1000
                 }
                 /*
                 SequentialAnimation  {
                     loops: Animation.Infinite
                     ColorAnimation { target: gradient1;  property: "color"; to: "white"; duration: 500 }
                     ColorAnimation { target: gradient1; property: "color"; to: "blue"; duration: 500  } }
                 SequentialAnimation  {
                     loops: Animation.Infinite
                     ColorAnimation { target: gradient2; property: "color"; to: "blue"; duration: 500 }
                     ColorAnimation { target: gradient2; property: "color"; to: "white"; duration: 500  } }*/


             },

             Transition {
                 from: "batteryfull"; to: "notusingbattery";

                 PropertyAnimation {
                     properties: "opacity"; duration:  1000
                 }
                 /*
                 SequentialAnimation  {
                     loops: Animation.Infinite
                     ColorAnimation { target: gradient1;  property: "color"; to: "white"; duration: 500 }
                     ColorAnimation { target: gradient1; property: "color"; to: "blue"; duration: 500  } }
                 SequentialAnimation  {
                     loops: Animation.Infinite
                     ColorAnimation { target: gradient2; property: "color"; to: "blue"; duration: 500 }
                     ColorAnimation { target: gradient2; property: "color"; to: "white"; duration: 500  } }*/


             },

             Transition {
                 from: "critical"; to: "notusingbattery";

                 /*SequentialAnimation  {
                     loops: Animation.Infinite
                     ColorAnimation { target: gradient1;  property: "color"; to: "white"; duration: 500 }
                     ColorAnimation { target: gradient1; property: "color"; to: "blue"; duration: 500  } }
                 SequentialAnimation  {
                     loops: Animation.Infinite
                     ColorAnimation { target: gradient2; property: "color"; to: "blue"; duration: 500 }
                     ColorAnimation { target: gradient2; property: "color"; to: "white"; duration: 500  } }*/
             }
         ]
         }

         Text {
             id: batterylevel
             anchors.centerIn: parent
             color: "black"
             font.bold: true
             text: value + '%'
         }

         Rectangle {
             id: batteryTip
             width:  5
             height: 19
             anchors { right: batteryRect.left; verticalCenter: batteryRect.verticalCenter }
             opacity: 0.5
             color: "black"

     }
     }
}





