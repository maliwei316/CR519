import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

Item {
    id: item1
    anchors.fill:parent
    property int thisPageNO: 1
    ColumnLayout {
        id: columnLayout
        x: 0
        y: 0
        width: parent.width/2
        height: parent.height
        spacing: 0

        RowLayout {
            id: rowLayout1
            width: 100
            height: parent.height/16
            spacing: 10

            Indicator2 {
                id: indicator210
                scale: 0.6
                objID: 210
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 0
            }

            Text {
                id: text8
                height: parent.height
                text: qsTr("I0.0")
                font.pixelSize: 14
            }

            Text {
                id: text7
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }





        }

        RowLayout {
            id: rowLayout2
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator211
                scale: 0.6
                objID: 211
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 1
            }

            Text {
                id: text9
                height: parent.height
                text: qsTr("I0.1")
                font.pixelSize: 14
            }

            Text {
                id: text10
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout3
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator212
                scale: 0.6
                objID: 212
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 2
            }

            Text {
                id: text11
                height: parent.height
                text: qsTr("I0.2")
                font.pixelSize: 14
            }

            Text {
                id: text12
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout4
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator213
                scale: 0.6
                objID: 213
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 3
            }

            Text {
                id: text13
                height: parent.height
                text: qsTr("I0.3")
                font.pixelSize: 14
            }

            Text {
                id: text14
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout5
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator214
                scale: 0.6
                objID: 214
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 4
            }

            Text {
                id: text15
                height: parent.height
                text: qsTr("I0.4")
                font.pixelSize: 14
            }

            Text {
                id: text16
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout6
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator215
                scale: 0.6
                objID: 215
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 5
            }

            Text {
                id: text17
                height: parent.height
                text: qsTr("I0.5")
                font.pixelSize: 14
            }

            Text {
                id: text18
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout7
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator216
                scale: 0.6
                objID: 216
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 6
            }

            Text {
                id: text19
                height: parent.height
                text: qsTr("I0.6")
                font.pixelSize: 14
            }

            Text {
                id: text20
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout8
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator217
                scale: 0.6
                objID: 217
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 0
                bitPosInByte: 7
            }

            Text {
                id: text21
                height: parent.height
                text: qsTr("I0.7")
                font.pixelSize: 14
            }

            Text {
                id: text22
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout9
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator218
                scale: 0.6
                objID: 218
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 0
            }

            Text {
                id: text23
                height: parent.height
                text: qsTr("I1.0")
                font.pixelSize: 14
            }

            Text {
                id: text24
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout10
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator219
                scale: 0.6
                objID: 219
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 1
            }

            Text {
                id: text25
                height: parent.height
                text: qsTr("I1.1")
                font.pixelSize: 14
            }

            Text {
                id: text26
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout11
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator220
                scale: 0.6
                objID: 220
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 2
            }

            Text {
                id: text27
                height: parent.height
                text: qsTr("I1.2")
                font.pixelSize: 14
            }

            Text {
                id: text28
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout12
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator221
                scale: 0.6
                objID: 221
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 3
            }

            Text {
                id: text29
                height: parent.height
                text: qsTr("I1.3")
                font.pixelSize: 14
            }

            Text {
                id: text30
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout13
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator222
                scale: 0.6
                objID: 222
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 4
            }

            Text {
                id: text31
                height: parent.height
                text: qsTr("I1.4")
                font.pixelSize: 14
            }

            Text {
                id: text32
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout14
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator223
                scale: 0.6
                objID: 223
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 5
            }

            Text {
                id: text33
                height: parent.height
                text: qsTr("I1.5")
                font.pixelSize: 14
            }

            Text {
                id: text34
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout29
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator238
                scale: 0.6
                objID: 238
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 6
            }

            Text {
                id: text63
                height: parent.height
                text: qsTr("I1.6")
                font.pixelSize: 14
            }

            Text {
                id: text64
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout30
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator239
                scale: 0.6
                objID: 239
                tableName: "PLC_DI"
                addressType: true
                byteAddress: 1
                bitPosInByte: 7
            }

            Text {
                id: text65
                height: parent.height
                text: qsTr("I1.7")
                font.pixelSize: 14
            }

            Text {
                id: text66
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }
    }

    ColumnLayout {
        id: columnLayout1
        anchors.left:columnLayout.right
        anchors.verticalCenter: columnLayout.verticalCenter
        width: parent.width/2
        height: parent.height
        spacing: 0
        RowLayout {
            id: rowLayout15
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator224
                scale: 0.6
                objID: 224
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 0
            }

            Text {
                id: text35
                height: parent.height
                text: qsTr("Q0.0")
                font.pixelSize: 14
            }

            Text {
                id: text36
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout16
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator225
                scale: 0.6
                objID: 225
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 1
            }

            Text {
                id: text37
                height: parent.height
                text: qsTr("Q0.1")
                font.pixelSize: 14
            }

            Text {
                id: text38
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout17
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator226
                scale: 0.6
                objID: 226
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 2
            }

            Text {
                id: text39
                height: parent.height
                text: qsTr("Q0.2")
                font.pixelSize: 14
            }

            Text {
                id: text40
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout18
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator227
                scale: 0.6
                objID: 227
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 3
            }

            Text {
                id: text41
                height: parent.height
                text: qsTr("Q0.3")
                font.pixelSize: 14
            }

            Text {
                id: text42
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout19
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator228
                scale: 0.6
                objID: 228
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 4
            }

            Text {
                id: text43
                height: parent.height
                text: qsTr("Q0.4")
                font.pixelSize: 14
            }

            Text {
                id: text44
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout20
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator229
                scale: 0.6
                objID: 229
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 5
            }

            Text {
                id: text45
                height: parent.height
                text: qsTr("Q0.5")
                font.pixelSize: 14
            }

            Text {
                id: text46
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout21
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator230
                scale: 0.6
                objID: 230
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 6
            }

            Text {
                id: text47
                height: parent.height
                text: qsTr("Q0.6")
                font.pixelSize: 14
            }

            Text {
                id: text48
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout22
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator231
                scale: 0.6
                objID: 231
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 7
            }

            Text {
                id: text49
                height: parent.height
                text: qsTr("Q0.7")
                font.pixelSize: 14
            }

            Text {
                id: text50
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout23
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator232
                scale: 0.6
                objID: 232
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 0
            }

            Text {
                id: text51
                height: parent.height
                text: qsTr("Q1.0")
                font.pixelSize: 14
            }

            Text {
                id: text52
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout24
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator233
                scale: 0.6
                objID: 233
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 1
            }

            Text {
                id: text53
                height: parent.height
                text: qsTr("Q1.1")
                font.pixelSize: 14
            }

            Text {
                id: text54
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout25
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator234
                scale: 0.6
                objID: 234
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 2
            }

            Text {
                id: text55
                height: parent.height
                text: qsTr("Q1.2")
                font.pixelSize: 14
            }

            Text {
                id: text56
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout26
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator235
                scale: 0.6
                objID: 235
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 3
            }

            Text {
                id: text57
                height: parent.height
                text: qsTr("Q1.3")
                font.pixelSize: 14
            }

            Text {
                id: text58
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout27
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator236
                scale: 0.6
                objID: 236
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 4
            }

            Text {
                id: text59
                height: parent.height
                text: qsTr("Q1.4")
                font.pixelSize: 14
            }

            Text {
                id: text60
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout28
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator237
                scale: 0.6
                objID: 237
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 5
            }

            Text {
                id: text61
                height: parent.height
                text: qsTr("Q1.5")
                font.pixelSize: 14
            }

            Text {
                id: text62
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout31
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator240
                scale: 0.6
                objID: 240
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 6
            }

            Text {
                id: text67
                height: parent.height
                text: qsTr("Q1.6")
                font.pixelSize: 14
            }

            Text {
                id: text68
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }

        RowLayout {
            id: rowLayout32
            width: 100
            height: parent.height/16
            Indicator2 {
                id: indicator241
                scale: 0.6
                objID: 241
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 1
                bitPosInByte: 7
            }

            Text {
                id: text69
                height: parent.height
                text: qsTr("Q1.7")
                font.pixelSize: 14
            }

            Text {
                id: text70
                text: qsTr("ESTOP Pressed")
                font.pixelSize: 14
            }
            spacing: 10
        }
    }

    Timer
    {
         id: timer2
         interval: 1000; repeat: true
         running: thisPageNO===tcpcomm1.PageNO?true:false
         triggeredOnStart: false

         onTriggered:
         {
               //readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address)
             //dbhc1.getValuefromDataBase(indicator21.objID,indicator21.tableName,indicator21.address);
             dbhc1.getValuefromDataBase(indicator21.objID,indicator21.tableName,indicator21.wordAddress)
             dbhc1.getValuefromDataBase(indicator22.objID,indicator22.tableName,indicator22.wordAddress);
             dbhc1.getValuefromDataBase(indicator23.objID,indicator23.tableName,indicator23.wordAddress);
             dbhc1.getValuefromDataBase(indicator24.objID,indicator24.tableName,indicator24.wordAddress);
             dbhc1.getValuefromDataBase(indicator25.objID,indicator25.tableName,indicator25.wordAddress);

          }

      }


    Connections {
         target: dbhc1
         //onDataReadyDBH2Controller2GUI:updateValue(reqesterID,address, value)
         onDataReadyDBH2Controller2GUI: updateValue(reqesterID,address, value)
     }
    function updateValue(reqesterID,address, value)
    {

        switch(reqesterID)
        {
        case indicator21.objID:
            if(address===indicator21.address)
                indicator21.value=value;
            break;
        case indicator22.objID:
            if(address===indicator22.address)
                indicator22.value=value;
            break;
        case indicator23.objID:
            if(address===indicator23.address)
                indicator23.value=value;
            break;
        case indicator24.objID:
            if(address===indicator24.address)
                indicator24.value=value;
            break;
        case indicator25.objID:
            if(address===indicator25.address)
            {

                indicator25.value=dbhc1.getBitsFromWord(value,indicator25.bitPosInWord);
            }

            break;
        default:
            break;
        }
     }







}
