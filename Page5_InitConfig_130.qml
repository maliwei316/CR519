
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
Item {
    width: 600
    height: 400
    readonly property int thisPageNO: 130
    TabView
    {
        id:tabview
        //height: parent.height
        //width: parent.width
        anchors.centerIn: parent
        anchors.fill: parent
        
        Tab
        {
            id:init1
            width: parent.width
            height: parent.height

            title: "init1";

            RowLayout {
                id: rowLayout
                width: 280
                height: parent.height
                anchors.fill: parent

                ColumnLayout {
                    id: columnLayout
                    x: 0
                    width: parent.width/2
                    height: parent.height
                    Layout.minimumHeight: 400
                    Layout.minimumWidth: 200
                    spacing: 10
                    anchors.margins: 20

                    RowLayout {
                        id: row_stationQty
                        y: 0
                        width: 200
                        height: parent.height/10
                        Text {
                            id: text3
                            text: qsTr("Station Qty")
                            font.pixelSize: 12
                            anchors.left: parent.left
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 30
                        }

                        Rectangle {
                            id: rectangle2
                            width: 30
                            height: 20
                            border.width: 2
                            anchors.right: parent.right
                            border.color: rectangle2.focus?"#000000":"#aabbcc"
                            TextInput {
                                id: textInput2
                                anchors.fill:parent
                                text: qsTr("")
                                readOnly: false
                                font.pixelSize: 12
                                Layout.minimumWidth: 20

                                Layout.minimumHeight: 20

                            }
                        }
                    }

                    RowLayout {
                        id: row_weldPointQty
                        y: 40
                        width: 200
                        //x:0
                        //y: 0

                        height: parent.height/10
                        //anchors.top:parent.top

                        Text {
                            id: text1
                            text: qsTr("WeldPoint Qty")
                            anchors.left: parent.left
                            Layout.minimumHeight: 30
                            Layout.minimumWidth: 100
                            font.pixelSize: 12
                        }

                        Rectangle {
                            id: rectangle
                            width: 30
                            height: 20
                            border.width: 2
                            anchors.right:parent.right
                            border.color: rectangle.focus?"#000000":"#aabbcc"
                            TextInput {
                                id: textInput
                                anchors.fill:parent
                                text: qsTr("")
                                selectionColor: "#ff800080"
                                z: 99
                                Layout.minimumHeight: 20
                                Layout.minimumWidth: 20
                                font.pixelSize: 12
                            }
                        }
                    }


                    RowLayout {
                        id: row_stepGroup
                        y: 80
                        width: 200
                        height: parent.height/10
                        Text {
                            id: text2
                            text: qsTr("Steps/Group Qty")
                            font.pixelSize: 12
                            anchors.left: parent.left
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 30
                        }

                        Rectangle {
                            id: rectangle1
                            width: 30
                            height: 20
                            border.width: 2
                            anchors.right: parent.right
                            border.color: rectangle1.focus?"#000000":"#aabbcc"
                            TextInput {
                                id: textInput1
                                text: qsTr("")
                                font.pixelSize: 12
                                Layout.minimumWidth: 20
                                anchors.fill: parent
                                Layout.minimumHeight: 20
                            }
                        }
                    }


                    RowLayout {
                        id: rowGeneratorQty
                        y: 120
                        width: 200
                        height: parent.height/10
                        Text {
                            id: text4
                            text: qsTr("Gens Qty")
                            font.pixelSize: 12
                            anchors.left: parent.left
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 30
                        }

                        Rectangle {
                            id: rectangle3
                            width: 30
                            height: 20
                            border.width: 2
                            anchors.right: parent.right
                            border.color: rectangle3.focus?"#000000":"#aabbcc"
                            TextInput {
                                id: textInput3
                                text: qsTr("")
                                font.pixelSize: 12
                                Layout.minimumWidth: 20
                                anchors.fill: parent
                                Layout.minimumHeight: 20
                            }
                        }
                    }

                    RowLayout {
                        id: row_ThrusterQty
                        y: 160
                        width: 200
                        height: parent.height/10
                        Text {
                            id: text5
                            text: qsTr("Thruster Qty")
                            font.pixelSize: 12
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 30
                        }

                        Rectangle {
                            id: rectangle4
                            width: 30
                            height: 20
                            border.width: 2
                            anchors.right: parent.right
                            border.color: rectangle4.focus?"#000000":"#aabbcc"
                            TextInput {
                                id: textInput4
                                text: qsTr("")
                                font.pixelSize: 12
                                Layout.minimumWidth: 20
                                anchors.fill: parent
                                Layout.minimumHeight: 20
                            }
                        }
                    }

                    RowLayout {
                        id: row_topHolderQty
                        y: 200
                        width: 200
                        height: parent.height/10
                        Text {
                            id: text6
                            text: qsTr("TopHolder Qty")
                            font.pixelSize: 12
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 30
                        }

                        Rectangle {
                            id: rectangle5
                            width: 30
                            height: 20
                            border.width: 2
                            anchors.right: parent.right
                            border.color: rectangle5.focus?"#000000":"#aabbcc"
                            TextInput {
                                id: textInput5
                                text: qsTr("")
                                font.pixelSize: 12
                                Layout.minimumWidth: 20
                                anchors.fill: parent
                                Layout.minimumHeight: 20
                            }
                        }
                    }

                    RowLayout {
                        id: rowPartClamperQty
                        y: 240
                        width: 200
                        height: parent.height/10
                        Text {
                            id: text7
                            anchors.verticalCenter: parent.verticalCenter
                            text: qsTr("PartClamper Qty")
                            font.pixelSize: 12
                            anchors.left: parent.left
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 30
                        }

                        Rectangle {
                            id: rectangle6
                            width: 30
                            height: 20
                            border.width: 2
                            anchors.right: parent.right
                            border.color: rectangle6.focus?"#000000":"#aabbcc"
                            TextInput {
                                id: textInput6
                                text: qsTr("")
                                font.pixelSize: 12
                                Layout.minimumWidth: 20
                                anchors.fill: parent
                                Layout.minimumHeight: 20
                            }
                        }
                    }
                    RowLayout {
                        id: rowtest1

                        width: parent.width
                        height: parent.height/8

                        Button{
                            id:initBbtn;

                            anchors.margins: 20
                            width: parent.width*0.8
                            height: parent.height
                            anchors.horizontalCenter: parent.horizontalCenter;

                            Text{

                                text: "Start Init";
                                anchors.centerIn: parent
                            }
                            onClicked: {

                                //mbclient1.setPLCCoilsTest();
                                //mbcc1.setCoils();
                            }
                        }
                    }


                }
            }

            
        }
        Tab
        {
            id:init2
            title:"init2";
            
            
        }
        
        
    }
    
}
