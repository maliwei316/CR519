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

typedef union
    {
    struct
                {
                    unsigned char B0;
                    unsigned char B1;
                    unsigned char B2;
                    unsigned char B3;

                }bytesVar;

        quint32 DWordVar;
    } dWordBytes;
typedef quint16 Word;
typedef quint32  DInt;
typedef quint8  Byte;
//QList<bytebits> bybitsList1;
//QList<wordBits>wordbitsList1;

typedef struct iQ_Settings
{

    //command byte B0
              bytebits B0_CommandByte;
              //unsigned char runweld:1;
              //unsigned char endweld:1;
              //unsigned char runtest:1;
              //unsigned char notuesed03:1;
              //unsigned char clearError:1;
              //unsigned char readActualBuffParmeters:1;
              //unsigned char output_data_valid:1;
              //unsigned char run_continous:1;
//communication status byte B1
              bytebits B1_iQLinQCommStatus;
              //unsigned char Link_Good_echo:1;
              //unsigned char Comm_error_echo:1;
              //unsigned char notused012:6;
//trig byte-process control B2
              bytebits B2_trigByte_ProcessControl;
                //unsigned char trigbyteprocesscontrol_0:1;
                //unsigned char trigbyteprocesscontrol_1:1;
                //unsigned char notused013:6;

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
              bytebits B20_trigByte_ProcLimits;
              //unsigned char trigbyteprocLimits_0:1;
              //unsigned char trigbyteprocLimits_1:1;
              //unsigned char notused014:6;
//B21
              bytebits B21_EnableSuspectBadByte;
              //unsigned char Enable_SuspectPartLimit:1;
              //unsigned char Enable_Bad_Part_Limits:1;
              //unsigned char notused015:6;
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
              bytebits B46_trigByteHW_Setup;
              //unsigned char TriggerByte_HW_Setup_0:1;
              //unsigned char TriggerByte_HW_Setup_1:1;
              //unsigned char notused016:6;

//B47
              bytebits B47_freqLockHold;
              //unsigned char  Freq_Lock_and_Hold:1;
              //unsigned char notused017:7;
              qint16 Neg20_to_Pos20;//B48_b49
              quint32 Free_Run_Frequency;//B50~B53
              quint32 Freq_Upper_Limit;//B54~B57
              quint32 Freq_Lower_Limit;//B58~B61
              Byte RESERVED_not_used;//B62
//trig byte-trig by power setup B63
              bytebits B63_trigByteTrigBypower;
              //unsigned char  TrigByteTrigByPower_0:1;
              //unsigned char  TrigByteTrigByPower_1:1;
              //unsigned char notused018:6;

 //B64
              bytebits B64_enableTrigByPower;
              //unsigned char Enable_TriggerByPower:1;
              //unsigned char notused019:7;

              Byte Trigger_Amplitude;//B65
              Word Trigger_Power;//B66-B67
              Word Trigger_Timeout;//B68-B69

} iQSettings;
typedef struct _weldPoint
{

    Byte pointNO;
    Byte stepNO;
    Byte ThrusterNO;
    Byte GenNO;

    iQSettings ultrasonicPara;
    Word thrusterPressure_down;
    Word thrusterPressure_up;
    Byte reservedByte1;
    Byte reservedByte2;

}weldPoint;

typedef struct genRealtimeData
{

        Byte amplitude;
        //Byte placeHolder1;
        Word power;
        DInt frequency;

}genRealTimeData;


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
//tcpCommVars* tcpvar1;
//tcpvar1=(tcpCommVars*)data.data();
//       qDebug()<<tr("tcpvar1 size :%1").arg(sizeof(*tcpvar1));

//       qDebug()<<tr("size of initiator in tcpvars:%1").arg(sizeof(tcpvar1->initiator));
//       qDebug()<<tr("size of commandNO in tcpvars:%1").arg(sizeof(tcpvar1->commandNO));
//       qDebug()<<tr("size of HMIPageNO in tcpvars:%1").arg(sizeof(tcpvar1->HMIPageNO));
//       qDebug()<<tr("size of toolID in tcpvars:%1").arg(sizeof(tcpvar1->toolID));
//       qDebug()<<tr("size of selectedPointNO in tcpvars:%1").arg(sizeof(tcpvar1->selectedPointNO));
//       qDebug()<<tr("size of selectedGenNO in tcpvars:%1").arg(sizeof(tcpvar1->selectedGenNO));
//      qDebug()<<tr("size of realTimeData_Gen1 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen1));
//       qDebug()<<tr("size of realTimeData_Gen2 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen2));
//       qDebug()<<tr("size of realTimeData_Gen3 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen3));
//       qDebug()<<tr("size of realTimeData_Gen4 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen4));
//       qDebug()<<tr("size of realTimeData_servo in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Servo1));
//       qDebug()<<tr("size of iQSettings in tcpvars:%1").arg(sizeof(tcpvar1->iQSettings1));
//       qDebug()<<tr("size of wordBits1 in tcpvars:%1").arg(sizeof(tcpvar1->wordBits1));
//       //qDebug()<<tr("size of byearray in tcpvars:%1").arg(sizeof(tcpvar1.));
//         qDebug()<<tr("size of byearray in tcpvars:%1").arg(sizeof(tcpvar1->byteArray));
//       //memcpy(&tcpvar1,data.data(),sizeof(data.data()));
//       qDebug()<<tr("tcpvar1 size :%1").arg(sizeof(*tcpvar1));
//       qDebug()<<"HMI PageNO:"<<tcpvar1->HMIPageNO;
//       qDebug()<<"Command NO:"<<tcpvar1->commandNO;
//       qDebug()<<"selected GenNO"<<tcpvar1->selectedGenNO;
//       qDebug()<<"selected Point NO:"<<tcpvar1->selectedPointNO;
//       qDebug()<<"Tool ID:"<<tcpvar1->toolID;
//       qDebug()<<"initiator:"<<tcpvar1->initiator;
//       qDebug()<<"gen1 amp:"<<tcpvar1->realTimeData_Gen1.amplitude;
//       qDebug()<<"gen1 power:"<<tcpvar1->realTimeData_Gen1.power;
//       qDebug()<<"gen1 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen1.power);
//       qDebug()<<"gen1 frq:"<<tcpvar1->realTimeData_Gen1.frequency;
//       qDebug()<<"gen1 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen1.frequency);
//       qDebug()<<"gen2 amp:"<<tcpvar1->realTimeData_Gen2.amplitude;
//       qDebug()<<"gen2 power:"<<tcpvar1->realTimeData_Gen2.power;
//       qDebug()<<"gen2 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen2.power);
//       qDebug()<<"gen2 frq:"<<tcpvar1->realTimeData_Gen2.frequency;
//       qDebug()<<"gen1 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen2.frequency);
//       qDebug()<<"gen3 amp:"<<tcpvar1->realTimeData_Gen3.amplitude;
//       qDebug()<<"gen3 power:"<<tcpvar1->realTimeData_Gen3.power;
//       qDebug()<<"gen3 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen3.power);
//       qDebug()<<"gen3 frq:"<<tcpvar1->realTimeData_Gen3.frequency;
//       qDebug()<<"gen3 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen3.frequency);
//       qDebug()<<"gen4 amp:"<<tcpvar1->realTimeData_Gen4.amplitude;
//       qDebug()<<"gen4 power:"<<tcpvar1->realTimeData_Gen4.power;
//       qDebug()<<"gen4 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen4.power);
//       qDebug()<<"gen4 frq:"<<tcpvar1->realTimeData_Gen4.frequency;
//       qDebug()<<"gen4 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen4.frequency);
//       qDebug()<<"servo speed:"<<tcpvar1->realTimeData_Servo1.speed;
//       qDebug()<<"servo speed(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Servo1.speed);
//       qDebug()<<"servo current pos:"<<tcpvar1->realTimeData_Servo1.position;
//       qDebug()<<"servo current pos(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Servo1.position);
//       qDebug()<<"servo target pos:"<<tcpvar1->realTimeData_Servo1.targetPos;
//       qDebug()<<"iQ Settings Amp:"<<tcpvar1->iQSettings1.Amplitude20to100;
//       qDebug()<<"free run frequency:"<<tcpvar1->iQSettings1.Free_Run_Frequency;
//       qDebug()<<"free run frequency(converted):"<<BigLittleSwap32(tcpvar1->iQSettings1.Free_Run_Frequency);
//void worker_modbus:: readReady1()
//{
//    auto reply = qobject_cast<QModbusReply *>(sender());//QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
//        if (!reply)
//            return;
//    //数据从QModbusReply这个类的result方法中获取,也就是本程序中的reply->result()

//        if (reply->error() == QModbusDevice::NoError)
//        {

//            QString sqlquery;
//            QString tableName;
//            const QModbusDataUnit unit = reply->result();
//            switch (unit.registerType()) {
//            case QModbusDataUnit::Coils:
//                tableName="PLC_DO";
//                break;
//            case QModbusDataUnit::DiscreteInputs:
//                tableName="PLC_DI";
//                break;
//            case QModbusDataUnit::HoldingRegisters:
//                if(this->port==502)
//                    tableName="PLC_M";
//                else if(this->port==503)
//                    tableName="PLC_DB";
//                break;
//            default:
//                tableName="";
//                break;
//            }
//            qint16 startAddress=unit.startAddress();
//            qint16 length=unit.valueCount();
//            qDebug()<<QTime::currentTime()<<"PLC reply data count:"<<length<<"table name:"<<tableName;
//           for (int i = 0; i < length; i++)
//           {
//                const QString entry = tr("address: %1,Value: %2").arg(startAddress+i)
//                                        .arg(QString::number(unit.value(i),
//                                              unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
//                //qDebug()<<"tableName:"<<tableName<<",reply from PLC "<<entry;
//                sqlquery=QObject::tr("insert or replace into %1(address,value) values(%2,%3)")
//                    .arg(tableName).arg(startAddress+i).arg(unit.value(i));
//            //post write query to DB handler,operate realTime DB
//            //myEvent_writeDB* e=new myEvent_writeDB((QEvent::Type)5001,sqlquery);
//            //QCoreApplication::postEvent(this,e);
//            qDebug()<<QTime::currentTime()<<tableName<<"-"<<i<<":sending out request of adding write DB into queue";
//            emit writeDatabaseRequired(sqlquery);

//           }
//           //QThread::msleep(50);
//           //QEventLoop eventloop;
//           //QTimer::singleShot(50, &eventloop, SLOT(quit()));
//           //eventloop.exec();
//        }
//        else if (reply->error() == QModbusDevice::ProtocolError)
//        {
//                      qDebug()<<tr("Read response error: %1 (Mobus exception: 0x%2)").
//                      arg(reply->errorString()).
//                      arg(reply->rawResult().exceptionCode(), -1, 16);
//        }
//        else {
//            qDebug()<<tr("Read response error: %1 (code: 0x%2)").
//                                        arg(reply->errorString()).
//                                        arg(reply->error(), -1, 16);
//        }

//        reply->deleteLater();


//}
//qDebug()<<"wp1.size:"<<sizeof(*wp1)<<sizeof(*data);
//qDebug()<<tr("size of iQSettings in WP1:%1").arg(sizeof(wp1->ultrasonicPara));

//qDebug()<<tr("size of wp1->pointNO in WP1:%1").arg(sizeof(wp1->pointNO));
//qDebug()<<tr("size of wp1->stepNO in WP1:%1").arg(sizeof(wp1->stepNO));
//qDebug()<<tr("size of wp1->ThrusterNO in WP1:%1").arg(sizeof(wp1->ThrusterNO));
//qDebug()<<tr("size of wp1->GenNO in WP1:%1").arg(sizeof(wp1->GenNO));
//qDebug()<<tr("size of wp1->thrusterPressure_down in WP1:%1").arg(sizeof(wp1->thrusterPressure_down));
//qDebug()<<tr("size of wp1->thrusterPressure_up in WP1:%1").arg(sizeof(wp1->thrusterPressure_up));
////qDebug()<<"wp1 to char*, data"<<data;//display will be null

//qDebug()<<"wp1 to QbyteArray , byte array's size:"<<ar1.size();
#pragma pack()
#endif // BITSOPERATION_H
