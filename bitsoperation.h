#ifndef BITSOPERATION_H
#define BITSOPERATION_H
//#include <stdio.h>
#include <QList>
#pragma pack(2)
 //这里就是要把数字x的第n位（bit（n-1）位）置为1
 //1U就表示的是无符号的1，宏定义可以传参的
 #define SET_BIT(x,n)    (x | 1U<<(n-1))

 //这里就是要把数字x的第n位（bit（n-1）位）清零
 #define CLEAR_BIT(x,n)    (x & ~(1U<<(n-1)))

//这里就是要把数字x的第n到m位置为1(n是低位，m是高位)
//就是先把0取反就可以得到很多的1，然后左移就得到那么多个0，再取反就可以得到你想要的1的个数了
//最后左移位或就可以置1了
#define SET_BITS(x,n,m)    (x | ~(~0U<<(m-n+1))<<(n-1))

//截取变量的部分连续位。(就是取出的意思)
//其实和上面那里是差不多的，后面那一大部分都是为了确定需要多少个1
//最后记得右移，为了得出那个数字
#define GET_BIT(x,n,m)    (x & ~(~0U<<(m-n+1))<<(n-1)) >>(n-1)


#define BigLittleSwap16(A)        ((((quint16)(A) & 0xff00) >> 8) | \
                                                       (((quint16)(A) & 0x00ff) << 8))


#define BigLittleSwap32(A)        ((((quint32)(A) & 0xff000000) >> 24) | \
                                                       (((quint32)(A) & 0x00ff0000) >> 8) | \
                                                       (((quint32)(A) & 0x0000ff00) << 8) | \
                                                       (((quint32)(A) & 0x000000ff) << 24))

#pragma pack(show)
typedef union
    {
    struct
                {
                    unsigned char b0:1;
                    unsigned char b1:1;
                    unsigned char b2:1;
                    unsigned char b3:1;
                    unsigned char b4:1;
                    unsigned char b5:1;
                    unsigned char b6:1;
                    unsigned char b7:1;
                }bits;

        unsigned char byteChar;
    } bytebits;
//#pragma pack()
//#pragma pack(2)
typedef union
    {
    struct
                {
                    unsigned char b8:1;
                    unsigned char b9:1;
                    unsigned char b10:1;
                    unsigned char b11:1;
                    unsigned char b12:1;
                    unsigned char b13:1;
                    unsigned char b14:1;
                    unsigned char b15:1;
                    unsigned char b0:1;
                    unsigned char b1:1;
                    unsigned char b2:1;
                    unsigned char b3:1;
                    unsigned char b4:1;
                    unsigned char b5:1;
                    unsigned char b6:1;
                    unsigned char b7:1;
                }bitsVar;
        quint16 wordVar;
    } wordBits;
//#pragma pack()

typedef quint16 Word;
typedef quint32  DInt;
typedef quint8  Byte;
//QList<bytebits> bybitsList1;
//QList<wordBits>wordbitsList1;
//#pragma pack(2)
#pragma pack(show)
typedef struct iQ_Settings
{

    //command byte B0
              unsigned char runweld:1;
              unsigned char endweld:1;
              unsigned char runtest:1;
              unsigned char notuesed03:1;
              unsigned char clearError:1;
              unsigned char readActualBuffParmeters:1;
              unsigned char output_data_valid:1;
              unsigned char run_continous:1;
//communication status byte B1
              unsigned char Link_Good_echo:1;
              unsigned char Comm_error_echo:1;
              unsigned char notused012:6;
//trig byte-process control B2
                unsigned char trigbyteprocesscontrol_0:1;
                unsigned char trigbyteprocesscontrol_1:1;
                unsigned char notused013:6;

              Byte Amplitude20to100;//B3
              Word Ramp_up_Time20to1250;//B4-B5
              Word Ramp_Down_TIme0to250;//B6-B7
              Word Weld_Time0to30000;//B8-B9
              Word Peak_Power;//B10-B11
              Word Energy0to65535;//B12-B13
              Word Hold_TIme0to30000;//B14-B15
              Word AfterBurstdelay0to999;//B16-B17
              Word Afterburst_Duration0t999;//B18-B19
//trig byte-process limits B20
              unsigned char trigbyteprocLimits_0:1;
              unsigned char trigbyteprocLimits_1:1;
              unsigned char notused014:6;
//B21
              unsigned char Enable_SuspectPartLimit:1;
              unsigned char Enable_Bad_Part_Limits:1;
              unsigned char notused015:6;
              Word Suspect_MaxTime_Limits;//B22-B23
              Word suspect_MinTime_Limits;//B24-B25
              Word Suspect_MaxPower_Limits;//B26-B27
              Word Suspect_MinPower_Limits;//B28-B29
              Word Suspect_MaxEnergy_Limits;//B30-B31
              Word Suspect_MinEnergy_Limits;//B32-B33
              Word BadPart_MaxTime_limits;//B34-B35
              Word Badpart_MinTime_Limits;//B36-B37
              Word BadPart_MaxPower_Limits;//B38-B39
              Word BadPart_MinPower_Limits;//B40-B41
              Word BadPart_MaxEnergy_Limits;//B42-B43
              Word BadPart_MinEnergy_Limits;//B44-B45
//trig byte-HW setup B46
              unsigned char TriggerByte_HW_Setup_0:1;
              unsigned char TriggerByte_HW_Setup_1:1;
              unsigned char notused016:6;

//B47
              unsigned char  Freq_Lock_and_Hold:1;
              unsigned char notused017:7;
              qint16 Neg20_to_Pos20;//B48_b49
              quint32 Free_Run_Frequency;//B50~B53
              quint32 Freq_Upper_Limit;//B54~B57
              quint32 Freq_Lower_Limit;//B58~B61
              Byte RESERVED_not_used;//B62
//trig byte-trig by power setup B63
              unsigned char  TrigByteTrigByPower_0:1;
              unsigned char  TrigByteTrigByPower_1:1;
              unsigned char notused018:6;

 //B64
              unsigned char Enable_TriggerByPower:1;
              unsigned char notused019:7;

              Byte Trigger_Amplitude;//B65
              Word Trigger_Power;//B66-B67
              Word Trigger_Timeout;//B68-B69

} iQSettings;
//#pragma pack(pop)
//#pragma pack(push,2)
#pragma pack(show)
typedef struct genRealtimeData
{

        Byte amplitude;
        //Byte placeHolder1;
        Word power;
        DInt frequency;

}genRealTimeData;
//#pragma pack()
//#pragma pack(2)
#pragma pack(show)
typedef struct servoRealtimeData
{

    //Word placeHooder1;
    //
    Byte targetPos;
    //Byte placeHoder2;
    //Byte placeHoder3;
    //Byte placeHoder4;
    DInt speed;
    //Word placeHooder3;
    //
    DInt position;

    //Byte placeHolder5;

}servoRealTimeData;
//#pragma pack(pop)
//#pragma pack(push,1)
#pragma pack(show)
typedef struct tcpCommStruct
{
        Byte initiator;
        Byte commandNO;
        Byte HMIPageNO;
        Byte toolID;
        Byte selectedPointNO;
        Byte selectedGenNO;
        genRealTimeData realTimeData_Gen1;
        genRealTimeData realTimeData_Gen2;
        genRealTimeData realTimeData_Gen3;
        genRealTimeData realTimeData_Gen4;
        servoRealTimeData realTimeData_Servo1;

        Byte byteArray[10];
        wordBits wordBits1;
        iQSettings iQSettings1;

}tcpCommVars;
#pragma pack()
#endif // BITSOPERATION_H
