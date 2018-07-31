import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
//import QtQuick.VirtualKeyboard 2.2

ApplicationWindow {
    id:window
    visible: true
    width: 800
    height: 600
    title: qsTr("CR519")
    readonly property int thisPageNO: 100
    SwipeView {
        id: swipeView
        anchors.top:parent.top

        currentIndex: tabBar.currentIndex
        width: parent.width
        height: parent.height*0.8
        Page1_Test_101 {

            ColorAnimation {
                from: "white"
                to: "black"
                duration: 200
            }

        }

        Page2_IO_Table_110 {}
        P130_InitConfig{
            Label {
                text: qsTr("Init page")
                anchors.centerIn: parent
            }

        }
        Page {
            Label {
                text: qsTr("Third page")
                anchors.centerIn: parent
            }
        }
        function mapBetweenPageNOAndIndex(page)
        {
            var returnIndex;
            switch(page)
            {
            case 101:
                returnIndex=0;
                break;
            case 110:
                returnIndex=1;
                break;
            case 120:
                returnIndex=2;
                break;

             default:
                 returnIndex=0;
                 break;
            }
            swipeView.currentIndex=returnIndex;

        }
        Connections
        {
          target: tcpcomm1;
          onPageNOChanged:swipeView.mapBetweenPageNOAndIndex(tcpcomm1.PageNO);
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("First")
            onClicked:tcpcomm1.PageNO=100;
        }
        TabButton {
            text: qsTr("Second")
            onClicked:tcpcomm1.PageNO=101;
        }
        TabButton {
            text: qsTr("Third")
            onClicked:tcpcomm1.PageNO=110;
        }
        TabButton {
            text: qsTr("Fourth")
            onClicked:tcpcomm1.PageNO=120;
        }
    }
//    InputPanel {
//        id: inputPanel
//        z: 99
//        x: 0
//        y: window.height
//        width: window.width

//        states: State {
//            name: "visible"
//            when: inputPanel.active
//            PropertyChanges {
//                target: inputPanel
//                y: window.height - inputPanel.height
//            }
//        }
//        transitions: Transition {
//            from: ""
//            to: "visible"
//            reversible: true
//            ParallelAnimation {
//                NumberAnimation {
//                    properties: "y"
//                    duration: 250
//                    easing.type: Easing.InOutQuad
//                }
//            }
//        }
//    }

}
