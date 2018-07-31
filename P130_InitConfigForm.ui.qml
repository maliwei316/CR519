import QtQuick 2.0
import QtQuick.Controls 2.3

Item {
    id: item1
    width: 400
    height: 400
    property alias button_loadfromFile1: button_loadfromFile1
    property alias button_execInit: button_execInit
    property alias button_loadfromFile: button_loadfromFile
    property alias maTextInputPartClamperQty: maTextInputPartClamperQty
    property alias text_CV_partClamper: text_CV_partClamper
    property alias text_CV_TopHolder: text_CV_TopHolder
    property alias maTextInputTopHolderQty: maTextInputTopHolderQty
    property alias maTextInputThrusterQty: maTextInputThrusterQty
    property alias text_CV_thruster: text_CV_thruster
    property alias maTextInputGenQty: maTextInputGenQty
    property alias text_CV_genQty: text_CV_genQty
    property alias text_CV_stepQty: text_CV_stepQty
    property alias maTextInputStepQty: maTextInputStepQty
    property alias text_CV_WeldPointQty: text_CV_WeldPointQty
    property alias maTextInputWeldPointQty: maTextInputWeldPointQty
    property alias text_CV_StationQty: text_CV_StationQty
    property alias maTextInput_stationQty: maTextInput_stationQty

    Rectangle {
        id: columnLayout
        width: parent.width
        height: parent.height

        Rectangle {
            id: rowLayout
            width: 400
            height: parent.height / 12
            anchors.top: parent.top
            anchors.topMargin: 0

            Text {
                id: text1
                text: qsTr("Name")
                anchors.leftMargin: 0
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 20
            }

            Text {
                id: text2
                text: qsTr("EditValue")
                font.wordSpacing: 5.5
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 20
            }

            Text {
                id: text3
                x: 200
                text: qsTr("CurrentValue")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10
                anchors.right: parent.right
                font.pixelSize: 20
            }
        }

        Rectangle {
            id: rowLayout1
            width: 400
            height: 33.333
            anchors.top: rowLayout.bottom
            anchors.topMargin: 10

            Text {
                id: text_StationQty
                width: 100
                height: 20
                text: qsTr("Station Qty")
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }

            MaTextInput {
                id: maTextInput_stationQty
            }

            Text {
                id: text_CV_StationQty
                text: qsTr("CV_StationQty")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 30
                anchors.right: parent.right
                font.pixelSize: 16
            }
        }

        Rectangle {
            id: rowLayout2
            width: 400
            height: parent.height / 12
            anchors.top: rowLayout1.bottom
            anchors.topMargin: 10

            Text {
                id: text_StationQty1
                width: 100
                height: 20
                text: qsTr("Weld Point Qty")
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }

            MaTextInput {
                id: maTextInputWeldPointQty
            }

            Text {
                id: text_CV_WeldPointQty
                text: qsTr("CV_WeldPointQty")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 30
                anchors.right: parent.right
                font.pixelSize: 16
            }
        }

        Rectangle {
            id: rowLayout3
            width: 400
            height: parent.height / 12
            anchors.top: rowLayout2.bottom
            anchors.topMargin: 10

            Text {
                id: text_StationQty2
                width: 100
                height: 20
                text: qsTr("Step Qty")
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }

            MaTextInput {
                id: maTextInputStepQty
            }

            Text {
                id: text_CV_stepQty
                text: qsTr("CV_stepQty")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 30
                anchors.right: parent.right
                font.pixelSize: 16
            }
        }

        Rectangle {
            id: rowLayout4
            width: 400
            height: parent.height / 12
            anchors.top: rowLayout3.bottom
            anchors.topMargin: 10

            Text {
                id: text_StationQty3
                width: 100
                height: 20
                text: qsTr("Generator Qty")
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }

            Text {
                id: text_CV_genQty
                text: qsTr("CV_genQty")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 30
                anchors.right: parent.right
                font.pixelSize: 16
            }

            MaTextInput {
                id: maTextInputGenQty
            }
        }

        Rectangle {
            id: rowLayout5
            width: 400
            height: parent.height / 12
            anchors.top: rowLayout4.bottom
            anchors.topMargin: 10

            Text {
                id: text_StationQty4
                width: 100
                height: 20
                text: qsTr("Thruster Qty")
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }

            Text {
                id: text_CV_thruster
                text: qsTr("CV_thruster")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 30
                anchors.right: parent.right
                font.pixelSize: 16
            }

            MaTextInput {
                id: maTextInputThrusterQty
            }
        }

        Rectangle {
            id: rowLayout6
            width: 400
            height: parent.height / 12
            anchors.top: rowLayout5.bottom
            anchors.topMargin: 10

            Text {
                id: text_StationQty5
                width: 100
                height: 20
                text: qsTr("Top Holder Qty")
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }

            MaTextInput {
                id: maTextInputTopHolderQty
            }

            Text {
                id: text_CV_TopHolder
                text: qsTr("CV_TopHolder")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 30
                anchors.right: parent.right
                font.pixelSize: 16
            }
        }

        Rectangle {
            id: rowLayout7
            width: 400
            height: parent.height / 12
            anchors.top: rowLayout6.bottom
            anchors.topMargin: 10

            Text {
                id: text_partClamper
                width: 100
                height: 20
                text: qsTr("PartClamper Qty")
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 16
            }

            Text {
                id: text_CV_partClamper
                text: qsTr("CV_partClamper")
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 30
                anchors.right: parent.right
                font.pixelSize: 16
            }

            MaTextInput {
                id: maTextInputPartClamperQty
            }
        }

        Rectangle {
            id: rowLayout8
            width: parent.width
            height: 40
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0

            Button {
                id: button_loadfromFile
                width: 100
                text: qsTr("Load from File")
                anchors.leftMargin: 0
                highlighted: true
            }

            Button {
                id: button_execInit
                width: 100
                text: qsTr("ExecInit")
                highlighted: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Button {
                id: button_loadfromFile1
                width: 100
                text: qsTr("Load from PLC")
                anchors.rightMargin: 0
                highlighted: true
                anchors.right: parent.right
            }
        }
    }
}
