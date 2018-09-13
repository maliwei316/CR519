#include "tcp_comm.h"
#include <QDebug>
#include <QTcpSocket>
#include "bitsoperation.h"
#include <QEventLoop>
#include <QTimer>
#include <QTime>

extern bool loggingEnable;
extern quint8 loggingLevel;

#pragma pack(1)

tcp_comm::tcp_comm(QString remoteIP,int portReceive_local, int portSend_local,int portReceive_remote, int portSend_remote, QObject *parent) : QObject(parent)
{

     //this->tcpServer=new QTcpServer(this);
     this->clientConnection_receive=new QTcpSocket(this);
     this->clientConnection_send=new QTcpSocket(this);
    if(this->clientConnection_send->bind(portSend_local))
    {
        qDebug()<<"bind to port successful,portSend_local:"<<portSend_local;
        this->clientConnection_send->connectToHost(remoteIP,portReceive_remote);
        if(this->clientConnection_send->waitForConnected(5000))
        {
            //this->clientConnection_send = tcpServer->nextPendingConnection();
            qDebug()<<"new connection detected,will send data via this port,peer port"<<this->clientConnection_send->peerPort();
            connect(clientConnection_send,&QTcpSocket::stateChanged,this,&tcp_comm::onStateChanged);
            connect(clientConnection_send, &QAbstractSocket::disconnected,clientConnection_send, &QObject::deleteLater);

        }
        else
        {
           qDebug()<<"wait for connection from PLC timeout, will have no ability to send specified data to PLC";
        }
    }
    else
    {
       qDebug()<<"bind to port fail,port:"<<portSend_local;
    }
    if(this->clientConnection_receive->bind(portReceive_local))
    {
        qDebug()<<"bind to port successful,receivePort:"<<portReceive_local;
        this->clientConnection_receive->connectToHost(remoteIP,portSend_remote);
        if(this->clientConnection_receive->waitForConnected(5000))
        {
            //this->clientConnection_receive = tcpServer->nextPendingConnection();
            qDebug()<<"new connection detected,will receive data from this port,peer port"<<this->clientConnection_receive->peerPort();
            connect(clientConnection_receive,&QTcpSocket::stateChanged,this,&tcp_comm::onStateChanged);
            connect(clientConnection_receive, &QAbstractSocket::disconnected,clientConnection_receive, &QObject::deleteLater);

            connect(clientConnection_receive,&QAbstractSocket::readyRead,this,&tcp_comm::onReadyRead);

        }
        else
        {
           qDebug()<<"wait for connection from PLC timeout, will have no ability to received specified data from PLC";
        }

    }
    else
    {
       qDebug()<<"bind to port fail,receive_port:"<<portReceive_local;
    }

//     //receive socket
//      if(tcpServer->isListening())
//         tcpServer->close();
//      if (!tcpServer->listen(QHostAddress::AnyIPv4,portReceive))
//      {
//          //qDebug()<<tr("Unable to start the server: %1.")
//                    //.arg(tcpServer->errorString());
//          if(loggingEnable&&loggingLevel>0)
//          {
//              QString logcontents=tr("Time:%1,Unable to start the server: %2")
//                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
//                      .arg(tcpServer->errorString());
//              emit this->logRequest(logcontents,100,0);
//          }

//      }
//      else

      //send socket
      //if(tcpServer->isListening())
          //tcpServer->close();
//      if (!tcpServer->listen(QHostAddress::AnyIPv4,portSend))
//      {
//          qDebug()<<tr("Unable to start the server: %1.")
//                    .arg(tcpServer->errorString());

//      }
//      else

      //report current connection status to mainWindow
      this->reportConnectionStatus();

}
void tcp_comm::reportConnectionStatus()
{
    //QString connectStatusInfo_send=this->clientConnection_send->state()==QAbstractSocket::ConnectedState?"Connected":"Disconnected";
    //QString connectStatusInfo_receive=this->clientConnection_receive->state()==QAbstractSocket::ConnectedState?"Connected":"Disconnected";

//    if(loggingEnable&&loggingLevel>0)
//    {
//        QString logcontents=tr("Time:%1,tcp comm state_send, current state: %2,tcp comm state_receive,current state:%3")
//                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd,hh:mm:ss.zzz"))
//                .arg(connectStatusInfo_send).arg(connectStatusInfo_receive);
//        emit this->logRequest(logcontents,105,0);
//    }
    //qDebug()<<"tcp comm state_send, current state:"<<this->clientConnection_send->state();
    //qDebug()<<"tcp comm state_send, current state:"<<this->clientConnection_receive->state();
    emit tcpCommConnectionStateChanged(this->clientConnection_send->state(),1);
    emit tcpCommConnectionStateChanged(this->clientConnection_receive->state(),2);
}
void tcp_comm::onReadyRead()
{
    if(this->clientConnection_receive->bytesAvailable()>=4)
    {
        if(receivedDataFromPLC.isEmpty())
            receivedDataFromPLC=this->clientConnection_receive->readAll();
        else
        {
            receivedDataFromPLC.append(this->clientConnection_receive->readAll());
        }
        quint16 validDataLenth=((quint8)receivedDataFromPLC[0])*256+((quint8)receivedDataFromPLC[1]);

        if(validDataLenth<0||validDataLenth>255*255)
        {
            qWarning()<<"valivd lenth is out of range,lenth:"<<validDataLenth;
            qDebug()<<"data from PLC, receivedData.size()"<<receivedDataFromPLC.size();
            for(int i=0;i<receivedDataFromPLC.count();i++)
            qDebug()<<tr("data from PLC, at[%1]:%2,data[%3]:%4,peer port:%5").arg(i).arg(QString::number(receivedDataFromPLC.at(i),16)).arg(i).arg(receivedDataFromPLC[i]).arg(this->clientConnection_receive->peerPort());

            receivedDataFromPLC.clear();
            return;
        }
        if(receivedDataFromPLC.size()<validDataLenth)
        {
            qDebug()<<tr("data from PLC, expect length:%1,current length:%2,will wait more data").arg(validDataLenth).arg(receivedDataFromPLC.size());

            return;
        }


       QByteArrayList receivedByteArrayList;

       if(receivedDataFromPLC.size()>4)
       {
           int subDataLength;
           int i=4;
           while(i<validDataLenth)
           {
             subDataLength=(quint8)receivedDataFromPLC[i]*256+(quint8)receivedDataFromPLC[i+1];
             qDebug()<<tr("rceived data.size:%1,valid dataLength:%2,index:%3,subdataLength:%4").arg(receivedDataFromPLC.size()).arg(validDataLenth).arg(i).arg((quint8)receivedDataFromPLC[i]*256+(quint8)receivedDataFromPLC[i+1]);
             if(subDataLength<6)
             {
                 qWarning()<<"subdata lenth less than 6, someting is wrong";
                 return;
             }
             receivedByteArrayList.append(receivedDataFromPLC.mid(i,subDataLength));
             i+=subDataLength;
           }
           for(int j=0;j<receivedByteArrayList.size();j++)
           {
               qDebug()<<"parsing received data,sub data ID:"<<j;
               this->parseDataFromPLC(receivedByteArrayList.at(j));
           }
           receivedDataFromPLC.clear();
       }

    }

}
void tcp_comm::parseDataFromPLC(const QByteArray& dataToParse)
{
    int dataToPase_size=dataToParse.size();
    if(dataToPase_size<6)
    {
       qDebug()<<"subData size <6,size:"<<dataToPase_size;
        return;
    }
    if(dataToPase_size!=(quint8)dataToParse.at(0)*256+dataToParse.at(1))
    {


       qDebug()<<tr("subData size not equal with length in byte array,subDatasize:%1,size of data to parse:%2")
                 .arg((quint8)dataToParse.at(0)*256+dataToParse.at(1)).arg(dataToPase_size);
       qDebug()<<tr("commandNO of first subData:%1,size of first subdata:%2")
                 .arg((quint8)dataToParse.at(8)*256+dataToParse.at(9)).arg((quint8)dataToParse.at(6)*256+dataToParse.at(7));

        return;
    }

    quint16 command=(quint8)dataToParse.at(2)*256+dataToParse.at(3);
    qDebug()<<"data to parse in tcpcommobg,command NO:"<<command;
    QByteArray dataLoad;
    if(dataToPase_size>6)
    {
      dataLoad=dataToParse.mid(6,dataToPase_size-1);
    }
    else
    {
      dataLoad.clear();
    }

    switch (command)
    {
    case 1://get generator real time data from PLC , then update the display value
        emit sendDataToWindow(dataToParse);

    case 2://get generator real time data from PLC , then update the display value
        emit sendDataToWindow(dataToParse);
        break;
    case 3://get generator real time data from PLC , then update the display value
        emit sendDataToWindow(dataToParse);
        break;
    case 5://get machine Qty config info from PLC , then update the display value
        emit sendDataToWindow(dataToParse);
        break;
    case 6:
        emit sendDataToWindow(dataToParse);
        break;
    case 7:
        emit sendDataToWindow(dataToParse);
        break;
    case 8:
        emit sendDataToWindow(dataToParse);
        break;
    case 9:
        emit sendDataToWindow(dataToParse);
        break;
    case 10:
        emit sendDataToWindow(dataToParse);
        break;
    case 11:
        emit sendDataToWindow(dataToParse);
        break;
    case 12:
        emit sendDataToWindow(dataToParse);
        break;
    case 13:
        emit sendDataToWindow(dataToParse);
        break;
    case 15:
        emit sendDataToWindow(dataToParse);
        break;
    case 16:
        emit sendDataToWindow(dataToParse);
        break;
    case 20:
    {

        for(int i=0;i<dataToPase_size;i++)
        {
            qDebug()<<tr("tcpcomm,point cycle data from PLC,data at[%1]:%2").arg(i).arg(dataToParse.at(i));
        }
        emit sendDataToWindow(dataToParse);
        break;
    }

    case 23:
        emit sendDataToWindow(dataToParse);
        break;

    default:
        qDebug()<<"Command not match , no assigned hanlder";
        break;
    }

}

void tcp_comm::receiveDataFromWindow(QByteArray dataFromWindow)
{
    this->prepareDataToPLC(dataFromWindow);
}

void tcp_comm::prepareDataToPLC(QByteArray newData)
{

    //dataToSend.clear();
    if(dataToSend.isEmpty())
    {
        for(int i=0;i<4;i++)
        dataToSend[i]=quint8(0x00);
    }

    //qDebug()<<"data before appendding new data:"<<this->dataToSend;
    //qDebug()<<"data to be appended"<<newData;
    this->dataToSend.append(newData);
    //this->dataToSend.enqueue(newData);
    //qDebug()<<"data after appendding new data:"<<this->dataToSend;
    quint16 length=this->dataToSend.size();
    this->dataToSend[0]=length/256;
    this->dataToSend[1]=length%256;
     qDebug()<<"length of sent bytes"<<length;
     //this->writeDataViaTCP(dataToSend);
      if(this->writeDataViaTCP(dataToSend))
         dataToSend.clear();

}

int tcp_comm::writeDataViaTCP(QByteArray dataToWrite)
{
    //qDebug()<<"writeDataViaTCP executed,data to write:"<<dataToWrite;
    int wroteCount=0;

    //dataToWrite[dataToWrite.count()-1]=(dataToWrite.at(dataToWrite.count()-1))?0x00:0xff;
   dataToWrite[2]=0x00;
   //dataToWrite[1]=4;
   qDebug()<<"writeDataViaTCP executed,data to write:"<<dataToWrite;
    wroteCount=this->clientConnection_send->write(dataToWrite);
    this->clientConnection_send->waitForBytesWritten();
    //dataToWrite.clear();
    qDebug()<<"writeDataViaTCP executed,wrote count:"<<wroteCount<<"peer port:"<<this->clientConnection_send->peerPort();
//    if(wroteCount)
//    this->isHavingToken=false;
    return wroteCount;
}

void tcp_comm::onStateChanged(QAbstractSocket::SocketState state)
{
   qDebug()<<"QTcpSocket state changed, current state:"<<state;

   this->reportConnectionStatus();


}
tcp_comm::~tcp_comm()
{
    this->tcpServer->deleteLater();

}

#pragma pack()

