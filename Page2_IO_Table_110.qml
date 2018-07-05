import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

Item {
    id: item1
    x:0
    y:0
    readonly property int thisPageNO: 100
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
                text: qsTr("Light Curtain OK")
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
                text: qsTr("SS1")
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
                text: qsTr("SS2")
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
                text: qsTr("Spare")
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
                text: qsTr("Servo Home Sensor")
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
                text: qsTr("Safety Door #1")
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
                text: qsTr("Safety Door #2")
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
                text: qsTr("L Fixture Clamp Btn ")
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
                text: qsTr("R Fixture Clamp Btn ")
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
                text: qsTr("Film Feeder Jog Btn")
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
                text: qsTr("Spare")
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
                text: qsTr("ESTOP&&Doors OK")
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
                text: qsTr("Comparessed Air OK")
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
                text: qsTr("Not Available")
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
                text: qsTr("Not Available")
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
                text: qsTr("PUL-Step Motor")
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
                text: qsTr("DIR-Step Motor")
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
                text: qsTr("SPARE")
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
                text: qsTr("SPARE")
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
                text: qsTr("Light Curtain Bypass")
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
                text: qsTr("Relay-Clamp#1 CoilA")
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
                text: qsTr("Relay-Clamp#1 CoilB")
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
                text: qsTr("Relay-Clamp#2 CoilA")
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
                text: qsTr("Relay-Clamp#2 CoilB")
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
                text: qsTr("Spare")
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
                text: qsTr("Not Available")
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
                text: qsTr("Not Available")
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
                text: qsTr("Not Available")
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
                text: qsTr("Not Available")
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
                text: qsTr("Not Available")
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
                text: qsTr("Not Available")
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
             dbhc1.getValuefromDataBase(indicator210.objID,indicator210.tableName,indicator210.wordAddress)
             dbhc1.getValuefromDataBase(indicator211.objID,indicator211.tableName,indicator211.wordAddress);
             dbhc1.getValuefromDataBase(indicator212.objID,indicator212.tableName,indicator212.wordAddress);
             dbhc1.getValuefromDataBase(indicator213.objID,indicator213.tableName,indicator213.wordAddress);
             dbhc1.getValuefromDataBase(indicator214.objID,indicator214.tableName,indicator214.wordAddress);

             dbhc1.getValuefromDataBase(indicator215.objID,indicator215.tableName,indicator215.wordAddress)
             dbhc1.getValuefromDataBase(indicator216.objID,indicator216.tableName,indicator216.wordAddress);
             dbhc1.getValuefromDataBase(indicator217.objID,indicator217.tableName,indicator217.wordAddress);
             dbhc1.getValuefromDataBase(indicator218.objID,indicator218.tableName,indicator218.wordAddress);
             dbhc1.getValuefromDataBase(indicator219.objID,indicator219.tableName,indicator219.wordAddress);

             dbhc1.getValuefromDataBase(indicator220.objID,indicator220.tableName,indicator220.wordAddress)
             dbhc1.getValuefromDataBase(indicator221.objID,indicator221.tableName,indicator221.wordAddress);
             dbhc1.getValuefromDataBase(indicator222.objID,indicator222.tableName,indicator222.wordAddress);
             dbhc1.getValuefromDataBase(indicator223.objID,indicator223.tableName,indicator223.wordAddress);
             dbhc1.getValuefromDataBase(indicator224.objID,indicator224.tableName,indicator224.wordAddress);

             dbhc1.getValuefromDataBase(indicator225.objID,indicator225.tableName,indicator225.wordAddress)
             dbhc1.getValuefromDataBase(indicator226.objID,indicator226.tableName,indicator226.wordAddress);
             dbhc1.getValuefromDataBase(indicator227.objID,indicator227.tableName,indicator227.wordAddress);
             dbhc1.getValuefromDataBase(indicator228.objID,indicator228.tableName,indicator228.wordAddress);
             dbhc1.getValuefromDataBase(indicator229.objID,indicator229.tableName,indicator229.wordAddress);

             dbhc1.getValuefromDataBase(indicator230.objID,indicator230.tableName,indicator230.wordAddress)
             dbhc1.getValuefromDataBase(indicator231.objID,indicator231.tableName,indicator231.wordAddress);
             dbhc1.getValuefromDataBase(indicator232.objID,indicator232.tableName,indicator232.wordAddress);
             dbhc1.getValuefromDataBase(indicator233.objID,indicator233.tableName,indicator233.wordAddress);
             dbhc1.getValuefromDataBase(indicator234.objID,indicator234.tableName,indicator234.wordAddress);

             dbhc1.getValuefromDataBase(indicator235.objID,indicator235.tableName,indicator235.wordAddress)
             dbhc1.getValuefromDataBase(indicator236.objID,indicator236.tableName,indicator236.wordAddress);
             dbhc1.getValuefromDataBase(indicator237.objID,indicator237.tableName,indicator237.wordAddress);
             dbhc1.getValuefromDataBase(indicator238.objID,indicator238.tableName,indicator238.wordAddress);
             dbhc1.getValuefromDataBase(indicator239.objID,indicator239.tableName,indicator239.wordAddress);

             dbhc1.getValuefromDataBase(indicator240.objID,indicator240.tableName,indicator240.wordAddress)
             dbhc1.getValuefromDataBase(indicator241.objID,indicator241.tableName,indicator241.wordAddress);
             //dbhc1.getValuefromDataBase(indicator232.objID,indicator232.tableName,indicator232.wordAddress);
             //dbhc1.getValuefromDataBase(indicator233.objID,indicator233.tableName,indicator233.wordAddress);
             //dbhc1.getValuefromDataBase(indicator234.objID,indicator234.tableName,indicator234.wordAddress);

             //dbhc1.getValuefromDataBase(indicator235.objID,indicator235.tableName,indicator235.wordAddress)
             //dbhc1.getValuefromDataBase(indicator236.objID,indicator236.tableName,indicator236.wordAddress);
             //dbhc1.getValuefromDataBase(indicator237.objID,indicator237.tableName,indicator237.wordAddress);
             //dbhc1.getValuefromDataBase(indicator238.objID,indicator238.tableName,indicator238.wordAddress);
             //dbhc1.getValuefromDataBase(indicator239.objID,indicator239.tableName,indicator239.wordAddress);
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
            case indicator210.objID:
                if(address===indicator210.wordAddress)
                    indicator210.value=value;
                break;
            case indicator211.objID:
                if(address===indicator211.wordAddress)
                    indicator211.value=value;
                break;
            case indicator212.objID:
                if(address===indicator212.wordAddress)
                    indicator212.value=value;
                break;
            case indicator213.objID:
                if(address===indicator213.wordAddress)
                    indicator213.value=value;
                break;
            case indicator214.objID:
                if(address===indicator214.wordAddress)
                    indicator214.value=value;
                break;
            case indicator215.objID:
                if(address===indicator215.wordAddress)
                    indicator215.value=value;
                break;
            case indicator216.objID:
                if(address===indicator216.wordAddress)
                    indicator216.value=value;
                break;
            case indicator217.objID:
                if(address===indicator217.wordAddress)
                    indicator217.value=value;
                break;
            case indicator218.objID:
                if(address===indicator218.wordAddress)
                    indicator218.value=value;
                break;
            case indicator219.objID:
                if(address===indicator219.wordAddress)
                    indicator219.value=value;
                break;
            //
           //220~229
            case indicator220.objID:
                if(address===indicator220.wordAddress)
                    indicator220.value=value;
                break;
            case indicator221.objID:
                if(address===indicator221.wordAddress)
                    indicator221.value=value;
                break;
            case indicator222.objID:
                if(address===indicator222.wordAddress)
                    indicator222.value=value;
                break;
            case indicator223.objID:
                if(address===indicator223.wordAddress)
                    indicator223.value=value;
                break;
            case indicator224.objID:
                if(address===indicator224.wordAddress)
                    indicator224.value=value;
                break;
            case indicator225.objID:
                if(address===indicator225.wordAddress)
                    indicator225.value=value;
                break;
            case indicator226.objID:
                if(address===indicator226.wordAddress)
                    indicator226.value=value;
                break;
            case indicator227.objID:
                if(address===indicator227.wordwordAddress)
                    indicator227.value=value;
                break;
            case indicator228.objID:
                if(address===indicator228.wordAddress)
                    indicator228.value=value;
                    console.log("indicator228 address:"+indicator228.wordAddress+"value:"+indicator228.value+"")

                break;
            case indicator229.objID:
                if(address===indicator229.wordAddress)
                    indicator229.value=value;
                break;
            //230~239
            case indicator230.objID:
                if(address===indicator230.wordAddress)
                    indicator230.value=value;
                break;
            case indicator231.objID:
                if(address===indicator231.wordAddress)
                    indicator231.value=value;
                break;
            case indicator232.objID:
                if(address===indicator232.wordAddress)
                    indicator232.value=value;
                break;
            case indicator233.objID:
                if(address===indicator233.wordAddress)
                    indicator233.value=value;
                break;
            case indicator234.objID:
                if(address===indicator234.wordAddress)
                    indicator234.value=value;
                break;
            case indicator235.objID:
                if(address===indicator235.wordAddress)
                    indicator235.value=value;
                break;
            case indicator236.objID:
                if(address===indicator236.wordAddress)
                    indicator236.value=value;
                break;
            case indicator237.objID:
                if(address===indicator237.wordAddress)
                    indicator237.value=value;
                break;
            case indicator238.objID:
                if(address===indicator238.wordAddress)
                    indicator238.value=value;
                break;
            case indicator239.objID:
                if(address===indicator239.wordAddress)
                    indicator239.value=value;
                break;
               //240~241
            case indicator240.objID:
                if(address===indicator240.wordAddress)
                    indicator240.value=value;
                break;
            case indicator241.objID:
                if(address===indicator241.wordAddress)
                    indicator241.value=value;
                break;

    //        case indicator25.objID:
    //            if(address===indicator25.wordAddress)
    //            {

    //                indicator25.value=dbhc1.getBitsFromWord(value,indicator25.bitPosInWord);
    //            }

               // break;
            default:
                break;
        }
     }







}
