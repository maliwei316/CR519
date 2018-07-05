import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4

Item {
       property int thisPageNO: 2
       ColumnLayout
       {
        anchors.fill: parent;

        RowLayout
        {
            id:row1
            spacing:100
            anchors.top: parent.top;
            Indicator2 {
                id:led1;
                objID: 100
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 1


            }
            Indicator2 {
                id:led2
                anchors.left: led1.right
                objID: 101
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 2
            }
            Indicator2 {
                id:led3
                anchors.left: led2.right
                objID: 102
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 3
            }
            Indicator2 {
                id:led4
                anchors.left: led3.right
                objID: 103
                tableName: "PLC_DO"
                addressType: true
                byteAddress: 0
                bitPosInByte: 4
            }
            Indicator2 {
                id:led5
                anchors.left: led4.right
                objID: 104
                tableName: "PLC_M"
                addressType: false
                byteAddress: 0
                bitPosInByte: 5
            }

        }
        RowLayout
        {
            id:row2
            anchors.topMargin: 20;
            width: row1.width
            anchors.top: row1.bottom
            Button{
                id:btn1;
                anchors.margins: 2
                onClicked: {timer1.running=timer1.running?false:true}
            }

        }

       }

       Timer
       {
            id: timer1
            interval: 1000; repeat: true
            running: thisPageNO===tcpcomm1.PageNO?true:false
            triggeredOnStart: false

            onTriggered:
            {
                  //readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address)
                //dbhc1.getValuefromDataBase(led1.objID,led1.tableName,led1.address);
                dbhc1.getValuefromDataBase(led1.objID,led1.tableName,led1.wordAddress)
                dbhc1.getValuefromDataBase(led2.objID,led2.tableName,led2.wordAddress);
                dbhc1.getValuefromDataBase(led3.objID,led3.tableName,led3.wordAddress);
                dbhc1.getValuefromDataBase(led4.objID,led4.tableName,led4.wordAddress);
                dbhc1.getValuefromDataBase(led5.objID,led5.tableName,led5.wordAddress);

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
           case led1.objID:
               if(address===led1.address)
                   led1.value=value;
               break;
           case led2.objID:
               if(address===led2.address)
                   led2.value=value;
               break;
           case led3.objID:
               if(address===led3.address)
                   led3.value=value;
               break;
           case led4.objID:
               if(address===led4.address)
                   led4.value=value;
               break;
           case led5.objID:
               if(address===led5.address)
               {

                   led5.value=dbhc1.getBitsFromWord(value,led5.bitPosInWord);
               }

               break;
           default:
               break;
           }
        }
}
