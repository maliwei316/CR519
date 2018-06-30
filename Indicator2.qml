import QtQuick 2.0

Rectangle{
    id:myIndicator2;
    height: 30;
    width: this.height;
    radius: this.height/2;
    //anchors.margins: this.width;
    //signal statusupdate(quint16 requesterObjID,QString tableName,quint16 address);
    property int objID: 0;
    property string tableName:"";
    property bool addressType: true; //DI,DO=true,Holding Register=false
    property int byteAddress: 0;
    property int bitPosInByte: 0;
    property int value: 0;
    property int wordAddress: calWordAddress();
    property int bitPosInWord: calBitPosInWord();
    color:calColor(value);
    //MouseArea{
    //anchors.fill: parent
    //onPressed: {value++;value%=5}
    //onReleased: {indicatorStatus=false;console.log(qsTr("Released"))}
    //}

    //function setValue(newValue){value=newValue}
    function calColor(value)
    {
      var calculatedColor;
        switch(value)
      {
        case 0:
            calculatedColor="gray";
            break;
        case 1:
            calculatedColor="light green";
            break;
        case 2:
            calculatedColor="yellow";
            break;
        case 3:
            calculatedColor="red";
            break;
        default:
            calculatedColor="blue";
            break;
      }
        return calculatedColor;
    }
    function calWordAddress()
    {
        if(addressType)
            return byteAddress*8+bitPosInByte;
        else
            return byteAddress/2;
    }
    function calBitPosInWord()
    {
        if(addressType)
            return 0;
        else
            return byteAddress%2?(bitPosInByte+8):bitPosInByte;
    }

//    onIndicatorStatusChanged: {
//        if (indicatorStatus)
//        myIndicator2.color="light green";
//        else
//          myIndicator2.color="gray";
//          //console.log(qsTr("statuschagned"))
//    }
         }
