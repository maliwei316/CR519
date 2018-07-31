import QtQuick 2.0

Rectangle {
    id: rectangle
    property int objID: 0;
    property string tableName:"";
    property bool addressType: true; //DI,DO=true,Holding Register=false
    property int byteAddress: 0;
    property int bitPosInByte: 0;
    property int bitValue: 0;
    property int wordValue: 0;
    property int wordAddress: calWordAddress();
    property int bitPosInWord: calBitPosInWord();
    property string valueToPLC: textInput.text;
    property alias validator:textInput.validator;
    property alias text:textInput.text;
    width: 30
    height: 20
    color: "#ffffff"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    border.color:"#ffeeee"
    border.width: 2

    TextInput {
        id: textInput
        text: qsTr("")
        anchors.fill: parent
        font.pixelSize: 16
        //cursorVisible: true

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
    //这里就是要把数字x的第n位（bit（n-1）位）置为1
    //1U就表示的是无符号的1，宏定义可以传参的
    //#define SET_BIT(x,n)    (x | 1U<<(n-1))

    //这里就是要把数字x的第n位（bit（n-1）位）清零
    //#define CLEAR_BIT(x,n)    (x & ~(1U<<(n-1)))
    function writePLC()
    {
        if(!addressType)
        {
            return textInput.text;
        }
        else
        {
            var valueToPLC;
            if(textInput.text)
                valueToPLC=wordValue|1<<(bitPosInWord-1);
            else
                valueToPLC=wordValue|~(1<<(bitPosInWord-1));
            return valueToPLC;
        }
    }
}
