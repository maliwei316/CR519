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
        height: parent.height/2
        Page1_Test_101 {

            ColorAnimation {
                from: "white"
                to: "black"
                duration: 200
            }

        }

        Page2_IO_Table_110 {

      }
        Page {
            Label {
                text: qsTr("Third page")
                anchors.centerIn: parent
            }
        }

    }
    RowLayout
    {
        id:rowlayout_buttonGP1
        spacing: 10
        anchors.top:swipeView.bottom
        width: swipeView.width
        Button{
            id:btn5;
            anchors.margins: 20
            //anchors.top: swipeView.bottom
            width: parent.width/6
            //anchors.bottom: tabBar.top;
            //anchors.left: tabBar.left;
            property bool languageFlag: false;
            Text{
                id:lang1
                text: (btn5.languageFlag)? (qsTr("Chinese")): (qsTr("English"));
                anchors.centerIn: parent
            }
            onClicked: {
                btn5.languageFlag=!btn5.languageFlag;

                if(btn5.languageFlag)
                    qmlLanguage.setLanguage(1);
                else
                   qmlLanguage.setLanguage(0);

            }
        }
        Button{
            id:btn6;
            anchors.margins: 20
            //anchors.left: btn5.right;
            anchors.verticalCenter: btn5.verticalCenter;
            width: btn5.width
            Text{
                id:setCoilsBtn
                text: "SetCoilsTest";
                anchors.centerIn: parent
            }
            onClicked: {

                //mbclient1.setPLCCoilsTest();
                mbcc1.setCoils();
            }
        }
        Button{
            id:btn7;
            anchors.margins: 20
            //anchors.left: btn6.right;
            anchors.verticalCenter: btn6.verticalCenter;
            width: btn5.width
            Text{
                id:resetCoilsBtn
                text: "ResetCoilsTest";
                anchors.centerIn: parent
            }
            onClicked: {

                //mbclient1.resetPLCCoilsTest();
                mbcc1.resetCoils();
            }
        }
       Button{
            id:btn8;
            anchors.margins: 20
            //anchors.left: btn8.right;
            anchors.verticalCenter: btn7.verticalCenter;
            width: btn5.width
            Text{
                id:checkConnectionBtn
                text: "CheckConnection";
                anchors.centerIn: parent
            }
            onClicked: {

                //mbclient1.checkConnection("checkbtn");
                //mbclient2.checkConnection("checkbtn");
                mbcc1.checkConnectionState();
                mbcc2.checkConnectionState();

            }
        }

    }

    RowLayout
    {
        id:rowlayout_buttonGP2
        spacing: 10
        anchors.top:rowlayout_buttonGP1.bottom
        anchors.topMargin: 10
        width: swipeView.width
        property int functionCode: 1
        property int controllerModbusSelection: 1

        ComboBox {
            id:combobox1
            width: 65

            height: 50
            textRole: "key"
            currentIndex: 1
            model: ListModel {
                id:listModle_Area
                ListElement { key: "DI"; value: 2 }
                ListElement { key: "DO"; value: 1 }
                ListElement { key: "M"; value: 3502 }
                ListElement { key: "DB"; value: 3503 }
            }
            onCurrentIndexChanged:function()
            {

              switch(listModle_Area.get(combobox1.currentIndex).value)
              {
              case 1:
                  rowlayout_buttonGP2.functionCode=1;
                  rowlayout_buttonGP2.controllerModbusSelection=1;
                  break;
              case 2:
                  rowlayout_buttonGP2.functionCode=2;
                  rowlayout_buttonGP2.controllerModbusSelection=1;
                  break;
              case 3502:
                  rowlayout_buttonGP2.functionCode=3;
                  rowlayout_buttonGP2.controllerModbusSelection=1;
                  break;
              case 3503:
                  rowlayout_buttonGP2.functionCode=3;
                  rowlayout_buttonGP2.controllerModbusSelection=2;
                  break;

              default:
                  break;
              }
            }
        }

        Text {
            id: text1
            anchors.left:combobox1.right
            text: qsTr("Start Address")
            font.pixelSize: 12
        }

        TextInput {
            id: startAddress
            validator: IntValidator{bottom: 0; top: 8192;}
            anchors.left:text1.right
            anchors.leftMargin: 10
            width: 80
            height: parent.height
            text: qsTr("0")
            font.pixelSize: 14
        }


        Text {
            id: text2
            anchors.left:startAddress.right
            anchors.leftMargin: 10
            text: qsTr("Length")
            font.pixelSize: 12
        }

        TextInput {
            id: length
            validator: IntValidator{bottom: 0; top: 1024;}
            anchors.left:text2.right
            anchors.leftMargin: 10
            width: 80
            height: 20
            text: qsTr("0")
            font.pixelSize: 14
        }



        Button {
            id: readbtn
            anchors.right:parent.right
            anchors.rightMargin: 10
            text: qsTr("Read")
            onClicked:
            {
                if(rowlayout_buttonGP2.controllerModbusSelection===1)
                    //mbcc1.readTest(rowlayout_buttonGP2.functionCode,startAddress.text,length.text);
                    ;

                else
                    //mbcc2.readTest(rowlayout_buttonGP2.functionCode,startAddress.text,length.text);
                    ;
            }
        }




    }
    Timer {
                 id: timer
                 interval: 200; repeat: false
                 running: thisPageNO===tcpcomm1.PageNO?true:false
                 triggeredOnStart: false

                 onTriggered: {
                     //var msg = {'action': 'appendCurrentTime', 'model': listModel};
                     //worker.sendMessage(msg);
                 }
             }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("First")
        }
        TabButton {
            text: qsTr("Second")
        }
        TabButton {
            text: qsTr("Third")
        }
        TabButton {
            text: qsTr("Fourth")
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
