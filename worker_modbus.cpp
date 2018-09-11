#include "worker_modbus.h"
#include <QTime>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QDataStream>
#include <QtSql>
#include <QEvent>
#include "myevent.h"
#include <QVariant>
#include <bitsoperation.h>

extern bool loggingEnable;
extern quint8 loggingLevel;

worker_modbus::worker_modbus(QObject *parent): QObject(parent)
{

}

void worker_modbus::onInit(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count)
{
    this->connectedFlag=false;
    this->DI_enable=false;
    this->DO_enable=false;
    this->HoldRegister_enable=false;
    this->DI_Var_count=0;
    this->DO_Var_count=0;
    this->HoldRegister_Var_count=0;
    this->IPAddr=IPAddr;
    this->port=port;
    this->timerEnableFlag=true;
    this->timer=new QTimer;
    this->setVarCounts(DI_Var_count,DO_Var_count,HoldRegister_Var_count);
    this->maModbusTcpClient=new QModbusTcpClient();
    this->maModbusTcpClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter,this->IPAddr);
    this->maModbusTcpClient->setConnectionParameter(QModbusDevice::NetworkPortParameter,this->port);
    this->connectPLC();
}
void worker_modbus::setVarCounts(int DI_Var_count,int DO_Var_count,int HoldRegister_Var_count)
{

    if(DI_Var_count>0)
    {this->DI_enable=true;this->DI_Var_count=DI_Var_count;}
    if(DO_Var_count>0)
    {this->DO_enable=true;this->DO_Var_count=DO_Var_count;}
    if(HoldRegister_Var_count>0)
    {this->HoldRegister_enable=true;this->HoldRegister_Var_count=HoldRegister_Var_count;}

}
bool worker_modbus::connectPLC()
{

    bool connectedStatus=false;

    if(this->maModbusTcpClient->connectDevice())
    {
        connectedStatus= true;
        connect(this->maModbusTcpClient, &QModbusTcpClient::stateChanged, this, &worker_modbus::onStateChanged);
        connect(this->maModbusTcpClient, &QModbusTcpClient::errorOccurred, this, &worker_modbus::onErrorOccurred);
        connect(this->maModbusTcpClient, &QModbusTcpClient::stateChanged, this, &worker_modbus::modbusStateChanged);
        connect(this->maModbusTcpClient, &QModbusTcpClient::errorOccurred, this, &worker_modbus::modbusErrorOccured);
    }

    else
    {
      connectedStatus=false;
      qDebug() << tr("failed to connected PLC via modbus,port:%1,error info:%2").arg(this->port)
                  .arg(this->maModbusTcpClient->errorString());
      if(loggingEnable&&loggingLevel>0)
      emit this->logRequest(this->maModbusTcpClient->errorString(),5,0);
    }

    if(connectedStatus)
    {

        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("Time:%1,connectting to PLC at %2 :%3 ")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz")).arg(this->IPAddr).arg(this->port);
            emit this->logRequest(logcontents,10,0);
        }

    }
    else
    {
      qDebug()<<QTime::currentTime()<<"failed to connect to PLC"<<this->IPAddr<<":"<<this->port;
      qDebug()<<"QModbusTcpClient State:"<<this->maModbusTcpClient->state();
      if(loggingEnable&&loggingLevel>0)
      {
          QString logcontents=tr("Time:%1,failed to connect to PLC at %2 :%3 ")
                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz")).arg(this->IPAddr).arg(this->port);
          emit this->logRequest(logcontents,15,0);
      }
    }
    return connectedStatus;
}
void worker_modbus::onStateChanged(QModbusDevice::State state)
{
  //qDebug()<<QTime::currentTime()<<"state changed:"<<state;
    QString strState;
    switch (state) {
    case QModbusDevice::ConnectedState:
        strState=tr("connected,IP:%1:%2").arg(this->IPAddr).arg(this->port);
        break;

    default:
        strState=tr("disconnected,IP:%1:%2").arg(this->IPAddr).arg(this->port);
        break;
    }
  if(loggingEnable&&loggingLevel>0)
  {
      QString logcontents=tr("Time:%1,state changed:%2")
              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz")).arg(strState);
      emit this->logRequest(logcontents,20,0);
  }

  if(state==QModbusDevice::ConnectedState)
      this->connectedFlag=true;
  else
      this->connectedFlag=false;
  if(this->connectedFlag)
  {

      QObject::connect(timer, &QTimer::timeout, this, &worker_modbus::readPLCByInterval);
      QEventLoop eventloop;

      QTimer::singleShot(500, &eventloop, SLOT(quit()));
      eventloop.exec();
      QThread::sleep(10);
      if(this->timerEnableFlag)
        timer->start(2000);
      //emit modbusConnected();

  }
  else
  {

      QObject::disconnect(timer, &QTimer::timeout, this, &worker_modbus::readPLCByInterval);
      timer->stop();
  }

}
void worker_modbus::onErrorOccurred(QModbusDevice::Error error)
{

  if(loggingEnable&&loggingLevel>0)
  {
      QString logcontents=tr("Time:%1,modbus error:%2")
              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz")).arg(this->maModbusTcpClient->errorString());
      emit this->logRequest(logcontents,25,0);
  }


}
void worker_modbus::readPLCByInterval()
{
    if(!(this->DI_enable||this->DO_enable||this->HoldRegister_enable))
    {
        this->timer->stop();
    }


    if(this->connectedFlag)
   {
//        if(loggingEnable&&loggingLevel>0)
//        {
//            QString logcontents=tr("Time:%1,read PLC by interval executed,IP:%2:%3")
//                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz")).arg(this->IPAddr).arg(this->port);
//            emit this->logRequest(logcontents,30,0);
//        }

       if(this->DO_enable)
       {
           //qDebug()<<QTime::currentTime()<<"DO reading started";
           this->readPLCCommand(1,0,DO_Var_count);
           //qDebug()<<QTime::currentTime()<<"DO reading done";
           QEventLoop eventloop;
           QTimer::singleShot(100, &eventloop, SLOT(quit()));
           eventloop.exec();
       }
       if(this->DI_enable)
       {
           //qDebug()<<QTime::currentTime()<<"DI reading started";
           this->readPLCCommand(2,0,DI_Var_count);
           //qDebug()<<QTime::currentTime()<<"DI reading done";
           QEventLoop eventloop;

           QTimer::singleShot(100, &eventloop, SLOT(quit()));
           eventloop.exec();
       }
       if(this->HoldRegister_enable)
       {
           //qDebug()<<QTime::currentTime()<<"M or DB reading started";
           this->readPLCCommand(3,0,HoldRegister_Var_count);
           //qDebug()<<QTime::currentTime()<<"M or DB reading done";
           QEventLoop eventloop;

           QTimer::singleShot(100, &eventloop, SLOT(quit()));
           eventloop.exec();
           QThread::msleep(200);
       }

   }
    else
    {
       //qDebug()<<"timer is running while modbus is not connected";
       if(loggingEnable&&loggingLevel>0)
       {
           QString logcontents=tr("Time:%1,timer is running while modbus is not connected,IP:%2:%3")
                   .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz")).arg(this->IPAddr).arg(this->port);
           emit this->logRequest(logcontents,30,0);
       }
    }
    //reset checking status
    this->checkingConnectionStatus=false;
}
void worker_modbus::readPLCCommand(quint16 functionCode,quint16 startAddress, quint16 length)
{

    if(!this->connectedFlag)
        return;
    if(functionCode==QModbusPdu::Invalid or functionCode>QModbusPdu::ReadInputRegisters)
    return;
    //qDebug()<<QTime::currentTime()<<"modbus connected,readPLCCommand executed ";
    QModbusDataUnit::RegisterType  dataUnitType;
    switch (functionCode)
    {

    case QModbusPdu::ReadCoils:
        dataUnitType=QModbusDataUnit::Coils;
        break;
    case QModbusPdu::ReadDiscreteInputs:
        dataUnitType=QModbusDataUnit::DiscreteInputs;
        break;
    case QModbusPdu::ReadHoldingRegisters:
        dataUnitType=QModbusDataUnit::HoldingRegisters;
        break;
    case QModbusPdu::ReadInputRegisters:
        dataUnitType=QModbusDataUnit::InputRegisters;
        break;
    default:
        break;
    }
    //qDebug()<<"dataUnitType:"<<dataUnitType;
    QModbusDataUnit readUnit(dataUnitType,startAddress,length);
    //qDebug()<<"line204 OK";
    if(auto* reply=this->maModbusTcpClient->sendReadRequest(readUnit,1))
    {


        if(!reply->isFinished())
           connect(reply, &QModbusReply::finished, this, &worker_modbus::readReady2);
        else
           delete reply;
    }
    else
    {
        qDebug()<<"reply is empty or error occured152";
        reply->deleteLater();

    }
//    QEventLoop eventloop;

//    QTimer::singleShot(20, &eventloop, SLOT(quit()));
//    eventloop.exec();
}
bool worker_modbus::waitReadPLCAtAddress(quint16 functionCode,quint16 Address, quint16 &result)
{
    if(!this->connectedFlag)
        return false;

    QModbusDataUnit::RegisterType  dataUnitType;
    if(functionCode==QModbusPdu::WriteSingleCoil)
    {
        dataUnitType=QModbusDataUnit::Coils;
    }
    else if(functionCode==QModbusPdu::WriteSingleRegister)
    {
        dataUnitType=QModbusDataUnit::HoldingRegisters;
    }

    else
        return false;
    QModbusDataUnit readUnit(dataUnitType,Address,1);
    //qDebug()<<"line204 OK";
    if(auto* reply=this->maModbusTcpClient->sendReadRequest(readUnit,1))
    {


        if(!reply->isFinished())
        {
            bool returnValue;
            int i=0;
            do{
                QEventLoop eventloop;
                QTimer::singleShot(10, &eventloop, SLOT(quit()));
                eventloop.exec();
                i++;
                if(i>30)
                {
                   qDebug()<<"wait PLC reply timeout";

                   returnValue= false;
                   break;
                }
            }while(!reply->isFinished());


            if(i<=30)
            {
                if (reply->error() == QModbusDevice::NoError)
                {
                    const QModbusDataUnit unit = reply->result();
                    result=unit.value(0);
                    returnValue=true;
                 }
                else if (reply->error() == QModbusDevice::ProtocolError)
                {
                      qDebug()<<tr("Read response error: %1 (Mobus exception: 0x%2)").
                      arg(reply->errorString()).
                      arg(reply->rawResult().exceptionCode(), -1, 16);
                      returnValue=false;
                }
                else {
                    qDebug()<<tr("Read response error: %1 (code: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->error(), -1, 16);
                    returnValue=false;
                }

            }
            else
            {

               returnValue=false;
            }
            return returnValue;
            reply->deleteLater();
        }

        else
           {
                 delete reply;
                return false;
            }


    }
    else
    {
        qDebug()<<"reply is empty or error occured152";
        reply->deleteLater();
        return false;

    }


}
void worker_modbus:: readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());//QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
        if (!reply)
            return;
    //数据从QModbusReply这个类的result方法中获取,也就是本程序中的reply->result()

        if (reply->error() == QModbusDevice::NoError)
        {

            //QString sqlquery;
            QString tableName;
            const QModbusDataUnit unit = reply->result();
            switch (unit.registerType()) {
            case QModbusDataUnit::Coils:
                tableName="PLC_DO";
                break;
            case QModbusDataUnit::DiscreteInputs:
                tableName="PLC_DI";
                break;
            case QModbusDataUnit::HoldingRegisters:
                if(this->port==502)
                    tableName="PLC_M";
                else if(this->port==503)
                    tableName="PLC_DB";
                break;
            default:
                tableName="";
                break;
            }
            qint16 startAddress=unit.startAddress();
            qint16 length=unit.valueCount();
            //qDebug()<<QTime::currentTime()<<"PLC reply data count:"<<length<<"table name:"<<tableName;
           QString prepareStr;
           QVariantList addressList,valueList;
           prepareStr=QObject::tr("insert or replace into %1(address,value) values(?,?)")
                .arg(tableName);
            for (int i = 0; i < length; i++)
           {
             addressList<<startAddress+i;
             valueList<<unit.value(i);
             /*const QString entry = tr("address: %1,Value: %2").arg(startAddress+i)
                                                      .arg(QString::number(unit.value(i),
                                                           unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));

             qDebug()<<"tableName:"<<tableName<<",reply from PLC "<<entry;*/
           }
            if(!addressList.isEmpty())
            emit this->batchWriteDataBaseRequired(prepareStr,addressList,valueList);
            //qDebug()<<QTime::currentTime()<<"sent out batch write DB signal,then sleep";
            QThread::msleep(50);
            QEventLoop eventloop;
            QTimer::singleShot(50, &eventloop, SLOT(quit()));
            eventloop.exec();
            //qDebug()<<QTime::currentTime()<<"waked";
        }
        else if (reply->error() == QModbusDevice::ProtocolError)
        {
                      qDebug()<<tr("Read response error: %1 (Mobus exception: 0x%2)").
                      arg(reply->errorString()).
                      arg(reply->rawResult().exceptionCode(), -1, 16);
        }
        else {
            qDebug()<<tr("Read response error: %1 (code: 0x%2)").
                                        arg(reply->errorString()).
                                        arg(reply->error(), -1, 16);
        }

        reply->deleteLater();


}
void worker_modbus:: readReady2()
{
    auto reply = qobject_cast<QModbusReply *>(sender());//QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
        if (!reply)
            return;
    //数据从QModbusReply这个类的result方法中获取,也就是本程序中的reply->result()

        if (reply->error() == QModbusDevice::NoError)
        {

            enum plcAera {DI=0x81, DO=0x82, M=0x83, DB=0x84};
            //QString sqlquery;
            QString tableName;
            quint8 area;
            const QModbusDataUnit unit = reply->result();
            switch (unit.registerType()) {
            case QModbusDataUnit::Coils:
                tableName="PLC_DO";
                area=plcAera::DO;
                break;
            case QModbusDataUnit::DiscreteInputs:
                tableName="PLC_DI";
                area=plcAera::DI;
                break;
            case QModbusDataUnit::HoldingRegisters:
                if(this->port==502)
                {
                    tableName="PLC_M";
                    area=plcAera::M;
                }

                else if(this->port==503)
                {
                    tableName="PLC_DB";
                    area=plcAera::DB;
                }

                break;
            default:
                tableName="";
                area=0;
                break;
            }
            qint16 startAddress=unit.startAddress();
            qint16 length=unit.valueCount();
            //qDebug()<<QTime::currentTime()<<tr("PLC reply data count:%1,tableName:%2,area:%3")
                   //.arg(length).arg(tableName).arg(area);
//            if(loggingEnable&&loggingLevel>0)
//            {
//                QString logcontents=tr("Time:%1,PLC reply data count:%2,tableName:%3,area:%4")
//                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
//                        .arg(length).arg(tableName).arg(area);
//                emit this->logRequest(logcontents,35,0);
//            }

           QVariantList addressList,valueList;

            for (int i = 0; i < length; i++)
           {
             addressList<<startAddress+i;
             valueList<<unit.value(i);
             /*const QString entry = tr("address: %1,Value: %2").arg(startAddress+i)
                                                      .arg(QString::number(unit.value(i),
                                                           unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));

             qDebug()<<"tableName:"<<tableName<<",reply from PLC "<<entry;*/
           }
            //call function to parse received data from PLC
            if(!addressList.isEmpty())
            {
               this->parseDataFromPLC(area,addressList,valueList);

            }


        }
        else if (reply->error() == QModbusDevice::ProtocolError)
        {
                      //qDebug()<<tr("Read response error: %1 (Mobus exception: 0x%2)").
                      //arg(reply->errorString()).
                      //arg(reply->rawResult().exceptionCode(), -1, 16);
                      if(loggingEnable&&loggingLevel>0)
                      {
                          QString logcontents=tr("Time:%1,Read response error: %2 (Mobus exception: 0x%3)")
                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
                                  .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                          emit this->logRequest(logcontents,35,0);
                      }

        }
        else {
            //qDebug()<<tr("Read response error: %1 (code: 0x%2)").
                                        //arg(reply->errorString()).
                                        //arg(reply->error(), -1, 16);
            if(loggingEnable&&loggingLevel>0)
            {
                QString logcontents=tr("Time:%1,Read response error: %2 (Mobus exception: 0x%3)")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
                        .arg(reply->errorString()).arg(reply->error(), -1, 16);
                emit this->logRequest(logcontents,35,0);
            }
        }

        reply->deleteLater();


}
void worker_modbus::parseDataFromPLC(quint8 area,QVariantList addressList,QVariantList valueList)
{
    //qDebug()<<"this->plcItemMap.size:"<<this->plcItemMap.size();
    QVariantList changedItems;

    plcItem item1;
    QVariant v;
    quint32 itemID;
    for(int i=0;i<addressList.size();i++)
    {
        item1.itemGroup_area=area;
        item1.wordAddress=addressList.at(i).toInt();
        itemID=item1.itemID();
        bool changed=false;
        changed=this->plcItemMap[itemID].updateValue(valueList.at(i).toInt());
        if(changed||this->checkingConnectionStatus)
        {
            this->plcItemMap[itemID].itemGroup_area=item1.itemGroup_area;
            this->plcItemMap[itemID].wordAddress=item1.wordAddress;
            v.setValue(this->plcItemMap[itemID]);
            changedItems.append(v);
        }
//        qDebug()<<tr("area:%1,itemID:%2,currentValue:%3,previousValue:%4, changed?:%5")
//                  .arg(plcItemMap[itemID].itemGroup_area)
//                  .arg(plcItemMap[itemID].itemID())
//                  .arg((qint16)plcItemMap[itemID].currentValue.wordVar)
//                  .arg((qint16)plcItemMap[itemID].previousValue.wordVar)
//                  .arg(plcItemMap[itemID].previousValue.wordVar==plcItemMap[itemID].currentValue.wordVar?"false":"true");

    }
  if(!changedItems.isEmpty())
  {
      emit this->plcItemsChanged(changedItems);
  }
}
void worker_modbus::writePLCCommand(quint16 functionCode, quint16 Address, const quint16 data, bool bitOperation, quint8 bitPos)
{
    if(!this->connectedFlag)
        return;

    QModbusDataUnit::RegisterType  dataUnitType;
    if(functionCode=QModbusPdu::WriteSingleCoil)
    {
        dataUnitType=QModbusDataUnit::Coils;
    }
    else if(functionCode=QModbusPdu::WriteSingleRegister)
    {
        dataUnitType=QModbusDataUnit::HoldingRegisters;
    }
    else
        return;

    qDebug()<<"dataUnitType:"<<dataUnitType;
    quint16 toWriteValue;
    if(bitOperation&&dataUnitType==QModbusDataUnit::HoldingRegisters)
    {
        //quint16 *currentValue;

//        if(this->waitReadPLCAtAddress(functionCode,Address,currentValue))
//        {
//            if(data)
//            {
//               toWriteValue=(*currentValue)|1U<<bitPos;//set bit
//            }
//            else
//            {
//               toWriteValue=(*currentValue)|~(1U<<bitPos);//reset bit
//            }
//        }
//        else
//        {
//            return;
//        }
    }
    else
        toWriteValue=data;

    QModbusDataUnit writeUnit(dataUnitType,Address,(quint16)1);
    QVector<quint16> tempVector(1);
    tempVector[0]=toWriteValue;
    writeUnit.setValues(tempVector);
    if (auto *reply = this->maModbusTcpClient->sendWriteRequest(writeUnit, 1)) {// 1是server address   sendWriteRequest是向服务器写数据
            if (!reply->isFinished())
            {   //reply Returns true when the reply has finished or was aborted.
                connect(reply, &QModbusReply::finished, this, [this, reply]() {
                    if (reply->error() == QModbusDevice::ProtocolError) {
                        qDebug()<<tr("Write response error: %1 (Mobus exception: 0x%2)")
                            .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                    } else if (reply->error() != QModbusDevice::NoError) {
                        qDebug()<<tr("Write response error: %1 (code: 0x%2)").
                            arg(reply->errorString()).arg(reply->error(), -1, 16);
                    }
                    reply->deleteLater();
                });
            }
            else {
               qDebug()<<"broadcast replies return immediately";
                reply->deleteLater();

            }
        }
    else {
            qDebug()<<"Write error:"<<this->maModbusTcpClient->errorString();
        }
}
void worker_modbus::onCheckConnectionStatus()
{
    //qDebug()<<QTime::currentTime()<<"checking modbus connection on going";
    QString connectStatusInfo;
    this->checkingConnectionStatus=true;
    switch (this->maModbusTcpClient->state()) {

    case QModbusDevice::UnconnectedState:

        connectStatusInfo="The device is disconnected";
        //this->connectPLC();

        break;
    case QModbusDevice::ConnectingState:
        connectStatusInfo="The device is being connected.";
        break;
    case QModbusDevice::ConnectedState:
        connectStatusInfo="The device is connected to the Modbus network";
        break;
    case QModbusDevice::ClosingState:
        connectStatusInfo="The device is being closed.";
        break;

    default:
        break;
    }
//    if(loggingEnable&&loggingLevel>0)
//    {
//        QString logcontents=tr("Time:%1,status: %2,IP:%3:%4")
//                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
//                .arg(connectStatusInfo).arg(this->IPAddr).arg(this->port);
//        emit this->logRequest(logcontents,35,0);
//    }
}
worker_modbus::~worker_modbus()
{
    this->maModbusTcpClient->disconnectDevice();
    this->maModbusTcpClient->deleteLater();
    timer->deleteLater();
}
void worker_modbus:: setPLCCoilsTest()
{
    QModbusDataUnit writeUnit(QModbusDataUnit::Coils,(quint16)0,(quint16)32);
    QVector<quint16> vector1(32);
    vector1.fill(1);
    vector1[4]=0;
    vector1[5]=0;
    vector1[6]=0;
    vector1[7]=0;
    writeUnit.setValues(vector1);
    if (auto *reply = this->maModbusTcpClient->sendWriteRequest(writeUnit, 1)) {// 1是server address   sendWriteRequest是向服务器写数据
            if (!reply->isFinished())
            {   //reply Returns true when the reply has finished or was aborted.
                connect(reply, &QModbusReply::finished, this, [this, reply]() {
                    if (reply->error() == QModbusDevice::ProtocolError) {
                        qDebug()<<tr("Write response error: %1 (Mobus exception: 0x%2)")
                            .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                    } else if (reply->error() != QModbusDevice::NoError) {
                        qDebug()<<tr("Write response error: %1 (code: 0x%2)").
                            arg(reply->errorString()).arg(reply->error(), -1, 16);
                    }
                    reply->deleteLater();
                });
            }
            else {
               qDebug()<<"broadcast replies return immediately180";
                reply->deleteLater();

            }
        }
    else {
            qDebug()<<"Write error:"<<this->maModbusTcpClient->errorString();
        }

}
void worker_modbus:: resetPLCCoilsTest()
{
    QModbusDataUnit writeUnit(QModbusDataUnit::Coils,(quint16)0,(quint16)32);
    QVector<quint16> vector1(32);
    vector1.fill(0);
    writeUnit.setValues(vector1);
    if (auto *reply = this->maModbusTcpClient->sendWriteRequest(writeUnit, 1)) {// 1是server address   sendWriteRequest是向服务器写数据
            if (!reply->isFinished())
            {   //reply Returns true when the reply has finished or was aborted.
                connect(reply, &QModbusReply::finished, this, [this, reply]() {
                    if (reply->error() == QModbusDevice::ProtocolError) {
                        qDebug()<<tr("Write response error: %1 (Mobus exception: 0x%2)")
                            .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16);
                    } else if (reply->error() != QModbusDevice::NoError) {
                        qDebug()<<tr("Write response error: %1 (code: 0x%2)").
                            arg(reply->errorString()).arg(reply->error(), -1, 16);
                    }
                    reply->deleteLater();
                });
            }
            else {
                if(loggingEnable&&loggingLevel>0)
                {
                    QString logcontents=tr("Time:%1,broadcast replies return immediately,IP:%2:%3")
                            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
                            .arg(this->IPAddr).arg(this->port);
                    emit this->logRequest(logcontents,35,0);
                }
                //qDebug()<<"broadcast replies return immediately210";
                reply->deleteLater();
            }
        }
    else {
            qDebug()<<"Write error:"<<this->maModbusTcpClient->errorString();
        }

}
