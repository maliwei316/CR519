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
worker_modbus::worker_modbus(QObject *parent): QObject(parent)
{}

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

    this->maModbusTcpClient=new QModbusTcpClient();
    bool connectedStatus=false;
    this->maModbusTcpClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter,this->IPAddr);
    this->maModbusTcpClient->setConnectionParameter(QModbusDevice::NetworkPortParameter,this->port);
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
      qDebug() << this->maModbusTcpClient->errorString();
    }

    if(connectedStatus)
    {
        qDebug()<<QTime::currentTime()<<"connectting to PLC at "<<this->IPAddr<<":"<<this->port;
        //qDebug()<<"this.thread"<<this->thread();
        //qDebug()<<"timer.thread(before moving)"<<timer->thread();
        //timer.moveToThread(this->thread());
        //qDebug()<<"timer.thread(after moving)"<<timer->thread();
        qDebug()<<"this->maModbusTcpClient.thread"<<this->maModbusTcpClient->thread();

    }
    else
    {
      qDebug()<<QTime::currentTime()<<"failed to connect to PLC"<<this->IPAddr<<":"<<this->port;
      qDebug()<<"QModbusTcpClient State:"<<this->maModbusTcpClient->state();
    }
    return connectedStatus;
}
void worker_modbus::onStateChanged(QModbusDevice::State state)
{
  qDebug()<<QTime::currentTime()<<"state changed:"<<state;
  if(state==QModbusDevice::ConnectedState)
      this->connectedFlag=true;
  else
      this->connectedFlag=false;
  if(this->connectedFlag)
  {

      QObject::connect(timer, &QTimer::timeout, this, &worker_modbus::readPLCtoRealtimeDB);
      QEventLoop eventloop;

      QTimer::singleShot(500, &eventloop, SLOT(quit()));
      eventloop.exec();
      QThread::sleep(10);
      if(this->timerEnableFlag)
        timer->start(1000);
      //emit modbusConnected();

  }
  else
  {
     QObject::disconnect(timer, &QTimer::timeout, this, &worker_modbus::readPLCtoRealtimeDB);
     timer->stop();
  }

}
void worker_modbus::onErrorOccurred(QModbusDevice::Error error)
{
  qDebug()<<"error occured:"<<error<<this->maModbusTcpClient->errorString();
}
void worker_modbus::readPLCtoRealtimeDB()
{

    if(this->connectedFlag)
   {
      qDebug()<<QTime::currentTime()<<"readPLCtoRealtimeDB executed,thread"<<QThread::currentThread();
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
       qDebug()<<"timer is running while modbus is not connected";
    }
}
void worker_modbus::readPLCCommand(quint16 functionCode,quint16 startAddress, quint16 length)
{

    if(!this->connectedFlag)
        return;
    if(functionCode==QModbusPdu::Invalid or functionCode>QModbusPdu::ReadInputRegisters)
    return;
    qDebug()<<QTime::currentTime()<<"modbus connected,readPLCCommand executed ";
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
    qDebug()<<"dataUnitType:"<<dataUnitType;
    QModbusDataUnit readUnit(dataUnitType,startAddress,length);
    //qDebug()<<"line204 OK";
    if(auto* reply=this->maModbusTcpClient->sendReadRequest(readUnit,1))
    {


        if(!reply->isFinished())
           connect(reply, &QModbusReply::finished, this, &worker_modbus::readReady);
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
void worker_modbus:: readReady1()
{
    auto reply = qobject_cast<QModbusReply *>(sender());//QModbusReply这个类存储了来自client的数据,sender()返回发送信号的对象的指针
        if (!reply)
            return;
    //数据从QModbusReply这个类的result方法中获取,也就是本程序中的reply->result()

        if (reply->error() == QModbusDevice::NoError)
        {

            QString sqlquery;
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
            qDebug()<<QTime::currentTime()<<"PLC reply data count:"<<length<<"table name:"<<tableName;
           for (int i = 0; i < length; i++)
           {
//                const QString entry = tr("address: %1,Value: %2").arg(startAddress+i)
//                                         .arg(QString::number(unit.value(i),
//                                              unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
                //qDebug()<<"tableName:"<<tableName<<",reply from PLC "<<entry;
                sqlquery=QObject::tr("insert or replace into %1(address,value) values(%2,%3)")
                    .arg(tableName).arg(startAddress+i).arg(unit.value(i));
            //post write query to DB handler,operate realTime DB
            //myEvent_writeDB* e=new myEvent_writeDB((QEvent::Type)5001,sqlquery);
            //QCoreApplication::postEvent(this,e);
            qDebug()<<QTime::currentTime()<<tableName<<"-"<<i<<":sending out request of adding write DB into queue";
            emit writeDatabaseRequired(sqlquery);

           }
           //QThread::msleep(50);
           //QEventLoop eventloop;
           //QTimer::singleShot(50, &eventloop, SLOT(quit()));
           //eventloop.exec();
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
            qDebug()<<QTime::currentTime()<<"PLC reply data count:"<<length<<"table name:"<<tableName;
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

void worker_modbus::checkConnection(QString info)
{
    qDebug()<<QTime::currentTime()<<"  check state:"<<info;

    switch (this->maModbusTcpClient->state()) {

    case QModbusDevice::UnconnectedState:

        qDebug("The device is disconnected");

        break;
    case QModbusDevice::ConnectingState:
        qDebug("The device is being connected.");
        break;
    case QModbusDevice::ConnectedState:
        qDebug("The device is connected to the Modbus network");
        break;
    case QModbusDevice::ClosingState:
        qDebug("The device is being closed.");
        break;

    default:
        break;
    }
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
               qDebug()<<"broadcast replies return immediately210";
                reply->deleteLater();

            }
        }
    else {
            qDebug()<<"Write error:"<<this->maModbusTcpClient->errorString();
        }

}
