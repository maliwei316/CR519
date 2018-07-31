import QtQuick 2.4

P130_InitConfigForm
{
    readonly property int thisPageNO: 130
    button_execInit.onClicked:
    {
        //void writePLC(quint16 functionCode, quint16 Address, const quint16 data,bool bitOperation,quint8 bitPosInWord);
        //mbcc2.writePLC(6,maTextInput_stationQty.wordAddress,maTextInput_stationQty.valueToPLC);//0x05 write coils,0x06 write hold register
        //mbcc2.writePLC(6,maTextInputGenQty.wordAddress,0,false,maTextInputGenQty.valueToPLC);
        //mbcc2.writePLC(6,0,1,true,8);//move 1 to word0.bit8(Byte0.0), execInit will activate
        maTextInputGenQty.text.toString();

    }
    maTextInput_stationQty.byteAddress: 2
    maTextInput_stationQty.addressType: false;
    maTextInput_stationQty.objID: 1301
    maTextInput_stationQty.tableName: "PLC_DB"
    maTextInput_stationQty.validator: IntValidator{bottom:1; top: 20}
    text_CV_StationQty.text: maTextInput_stationQty.addressType?maTextInput_stationQty.bitValue:maTextInput_stationQty.wordValue
    maTextInputGenQty.byteAddress: 4
    maTextInputGenQty.addressType: false;
    maTextInputGenQty.objID: 1302
    maTextInputGenQty.tableName: "PLC_DB"
    maTextInputGenQty.validator: IntValidator{bottom:1; top: 4}
    text_CV_genQty.text: maTextInputGenQty.addressType?maTextInputGenQty.bitValue:maTextInputGenQty.wordValue

    Timer
    {
         id: timer130
         interval: 1000; repeat: true
         running: thisPageNO===tcpcomm1.PageNO?true:false
         triggeredOnStart: false

         onTriggered:
         {
               //readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address)
             //dbhc1.getValuefromDataBase(indicator21.objID,indicator21.tableName,indicator21.address);
             dbhc1.getValuefromDataBase(maTextInput_stationQty.objID,maTextInput_stationQty.tableName,maTextInput_stationQty.wordAddress)
             dbhc1.getValuefromDataBase(maTextInputGenQty.objID,maTextInputGenQty.tableName,maTextInputGenQty.wordAddress);


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

            case maTextInput_stationQty.objID:
                if(address===maTextInput_stationQty.wordAddress)
                {

                    maTextInput_stationQty.wordValue=value;
                    if(maTextInput_stationQty.addressType)
                    maTextInput_stationQty.bitValue=dbhc1.getBitsFromWord(value,maTextInput_stationQty.bitPosInWord);
                }

                break;
            case maTextInputGenQty.objID:
                if(address===maTextInputGenQty.wordAddress)
                {

                    maTextInputGenQty.wordValue=value;
                    if(maTextInputGenQty.addressType)
                    maTextInputGenQty.bitValue=dbhc1.getBitsFromWord(value,maTextInputGenQty.bitPosInWord);
                }

                break;
            default:
                break;
        }
     }



}
