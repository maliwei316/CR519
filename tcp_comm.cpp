#include "tcp_comm.h"
#include <QDebug>
#include <QTcpSocket>
#include "bitsoperation.h"
#pragma pack(1)

tcp_comm::tcp_comm(int portReceive, int portSend,QObject *parent) : QObject(parent)
{
      this->tcpServer=new QTcpServer(this);
      if (!tcpServer->listen(QHostAddress::Any,portReceive)) {
          qDebug()<<tr("Unable to start the server: %1.")
                    .arg(tcpServer->errorString());

    }
      else
      {
          if(tcpServer->waitForNewConnection(10000))
          {
              this->clientConnection_receive = tcpServer->nextPendingConnection();
              qDebug()<<"new connection detected,will receive data from this port,peer port"<<this->clientConnection_receive->peerPort();
              connect(clientConnection_receive,&QTcpSocket::stateChanged,this,&tcp_comm::onStateChanged);
              connect(clientConnection_receive, &QAbstractSocket::disconnected,clientConnection_receive, &QObject::deleteLater);
              //clientConnection->write(block);
              //clientConnection->disconnectFromHost();
              connect(clientConnection_receive,&QAbstractSocket::readyRead,this,&tcp_comm::onReadyRead);

          }
          else
          {
             qDebug()<<"wait for connection from PLC timeout, will have no ability to received specified data from PLC";
          }
          //qDebug()<<"listening?"<<tcpServer->isListening();
          //connect(tcpServer, &QTcpServer::newConnection, this, &tcp_comm::onNewConnection);


      }
    if(this->clientConnection_receive)
    {
        tcpServer->close();
        if (!tcpServer->listen(QHostAddress::Any,portSend)) {
            qDebug()<<tr("Unable to start the server: %1.")
                      .arg(tcpServer->errorString());

      }
        else
        {
            if(tcpServer->waitForNewConnection(10000))
            {
                this->clientConnection_send = tcpServer->nextPendingConnection();
                qDebug()<<"new connection detected,will send data via this port,peer port"<<this->clientConnection_send->peerPort();
                connect(clientConnection_send,&QTcpSocket::stateChanged,this,&tcp_comm::onStateChanged);
                connect(clientConnection_send, &QAbstractSocket::disconnected,clientConnection_send, &QObject::deleteLater);
                //connect(clientConnection_receive,&QAbstractSocket::readyRead,this,&tcp_comm::onReadyRead);

            }
            else
            {
               qDebug()<<"wait for connection from PLC timeout, will have no ability to send specified data to PLC";
            }
        }
}


}
void tcp_comm::onReadyRead()
{
    if(this->clientConnection_receive->bytesAvailable()>=130)
    {
        data=this->clientConnection_receive->readAll();
        qDebug()<<"data.count"<<data.count();
        qDebug()<<"data.size()"<<data.size();
       for(int i=0;i<data.count();i++)
       qDebug()<<tr("data from PLC, at[%1]:%2,data[%3]:%4,peer port:%5").arg(i).arg(QString::number(data.at(i),16)).arg(i).arg(data[i]).arg(this->clientConnection_receive->peerPort());
       tcpCommVars* tcpvar1;
       tcpvar1=(tcpCommVars*)data.data();
       qDebug()<<tr("tcpvar1 size :%1").arg(sizeof(*tcpvar1));

       qDebug()<<tr("size of initiator in tcpvars:%1").arg(sizeof(tcpvar1->initiator));
       qDebug()<<tr("size of commandNO in tcpvars:%1").arg(sizeof(tcpvar1->commandNO));
       qDebug()<<tr("size of HMIPageNO in tcpvars:%1").arg(sizeof(tcpvar1->HMIPageNO));
       qDebug()<<tr("size of toolID in tcpvars:%1").arg(sizeof(tcpvar1->toolID));
       qDebug()<<tr("size of selectedPointNO in tcpvars:%1").arg(sizeof(tcpvar1->selectedPointNO));
       qDebug()<<tr("size of selectedGenNO in tcpvars:%1").arg(sizeof(tcpvar1->selectedGenNO));
      qDebug()<<tr("size of realTimeData_Gen1 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen1));
       qDebug()<<tr("size of realTimeData_Gen2 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen2));
       qDebug()<<tr("size of realTimeData_Gen3 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen3));
       qDebug()<<tr("size of realTimeData_Gen4 in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Gen4));
       qDebug()<<tr("size of realTimeData_servo in tcpvars:%1").arg(sizeof(tcpvar1->realTimeData_Servo1));
       qDebug()<<tr("size of iQSettings in tcpvars:%1").arg(sizeof(tcpvar1->iQSettings1));
       qDebug()<<tr("size of wordBits1 in tcpvars:%1").arg(sizeof(tcpvar1->wordBits1));
       //qDebug()<<tr("size of byearray in tcpvars:%1").arg(sizeof(tcpvar1.));
         qDebug()<<tr("size of byearray in tcpvars:%1").arg(sizeof(tcpvar1->byteArray));
       //memcpy(&tcpvar1,data.data(),sizeof(data.data()));
       qDebug()<<tr("tcpvar1 size :%1").arg(sizeof(*tcpvar1));


       qDebug()<<"HMI PageNO:"<<tcpvar1->HMIPageNO;
       qDebug()<<"Command NO:"<<tcpvar1->commandNO;
       qDebug()<<"selected GenNO"<<tcpvar1->selectedGenNO;
       qDebug()<<"selected Point NO:"<<tcpvar1->selectedPointNO;
       qDebug()<<"Tool ID:"<<tcpvar1->toolID;
       qDebug()<<"initiator:"<<tcpvar1->initiator;
       qDebug()<<"gen1 amp:"<<tcpvar1->realTimeData_Gen1.amplitude;
       qDebug()<<"gen1 power:"<<tcpvar1->realTimeData_Gen1.power;
       qDebug()<<"gen1 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen1.power);
       qDebug()<<"gen1 frq:"<<tcpvar1->realTimeData_Gen1.frequency;
       qDebug()<<"gen1 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen1.frequency);
       qDebug()<<"gen2 amp:"<<tcpvar1->realTimeData_Gen2.amplitude;
       qDebug()<<"gen2 power:"<<tcpvar1->realTimeData_Gen2.power;
       qDebug()<<"gen2 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen2.power);
       qDebug()<<"gen2 frq:"<<tcpvar1->realTimeData_Gen2.frequency;
       qDebug()<<"gen1 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen2.frequency);
       qDebug()<<"gen3 amp:"<<tcpvar1->realTimeData_Gen3.amplitude;
       qDebug()<<"gen3 power:"<<tcpvar1->realTimeData_Gen3.power;
       qDebug()<<"gen3 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen3.power);
       qDebug()<<"gen3 frq:"<<tcpvar1->realTimeData_Gen3.frequency;
       qDebug()<<"gen3 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen3.frequency);
       qDebug()<<"gen4 amp:"<<tcpvar1->realTimeData_Gen4.amplitude;
       qDebug()<<"gen4 power:"<<tcpvar1->realTimeData_Gen4.power;
       qDebug()<<"gen4 power(converted):"<<BigLittleSwap16(tcpvar1->realTimeData_Gen4.power);
       qDebug()<<"gen4 frq:"<<tcpvar1->realTimeData_Gen4.frequency;
       qDebug()<<"gen4 frq(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Gen4.frequency);
       qDebug()<<"servo speed:"<<tcpvar1->realTimeData_Servo1.speed;
       qDebug()<<"servo speed(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Servo1.speed);
       qDebug()<<"servo current pos:"<<tcpvar1->realTimeData_Servo1.position;
       qDebug()<<"servo current pos(converted):"<<BigLittleSwap32(tcpvar1->realTimeData_Servo1.position);
       qDebug()<<"servo target pos:"<<tcpvar1->realTimeData_Servo1.targetPos;
       qDebug()<<"iQ Settings Amp:"<<tcpvar1->iQSettings1.Amplitude20to100;
       qDebug()<<"free run frequency:"<<tcpvar1->iQSettings1.Free_Run_Frequency;
       qDebug()<<"free run frequency(converted):"<<BigLittleSwap32(tcpvar1->iQSettings1.Free_Run_Frequency);


       for(int i=0;i<10;i++)
       {
          qDebug()<<tr("byte array at[%1]:").arg(i)<<tcpvar1->byteArray[i];
       }

       //emit writeBackReceivedData(data);
    }
    else
    {
        qDebug()<<"too less data received";
    }
}
void tcp_comm::onStateChanged(QAbstractSocket::SocketState state)
{
   qDebug()<<"QTcpSocket state changed, current state:"<<state;
}
int tcp_comm::getCurrentPageNO()
{
  return this->pageNO;
}
void tcp_comm::setCurrentPageNO(int newPageNO)
{
   if(this->pageNO!=newPageNO)
   {
       this->pageNO=newPageNO;
       emit this->pageNOChanged(this->pageNO);
   }
}
int tcp_comm::writeDataViaTCP(QByteArray dataToWrite)
{
    qDebug()<<"writeDataViaTCP executed,data to write:"<<dataToWrite;
    int wroteCount=0;

    dataToWrite[dataToWrite.count()-1]=(dataToWrite.at(dataToWrite.count()-1))?0x00:0xff;

    wroteCount=this->clientConnection_send->write(dataToWrite);
    qDebug()<<"writeDataViaTCP executed,wrote count:"<<wroteCount<<"peer port:"<<this->clientConnection_send->peerPort();
    return wroteCount;
}
tcp_comm::~tcp_comm()
{
    this->tcpServer->deleteLater();

}
//void tcp_comm::onNewConnection()
//{

//    //QByteArray block;
//    //QDataStream out(&block, QIODevice::WriteOnly);
//    //out.setVersion(QDataStream::Qt_5_10);

//   // out << fortunes[QRandomGenerator::global()->bounded(fortunes.size())];

//    this->clientConnection = tcpServer->nextPendingConnection();
//    qDebug()<<"new connection detected,peer port"<<this->clientConnection->peerPort();
//    connect(clientConnection,&QTcpSocket::stateChanged,this,&tcp_comm::onStateCahnged);
//    connect(clientConnection, &QAbstractSocket::disconnected,clientConnection, &QObject::deleteLater);
//    //clientConnection->write(block);
//    //clientConnection->disconnectFromHost();
//    connect(clientConnection,&QAbstractSocket::readyRead,this,&tcp_comm::onReadyRead);
//}
#pragma pack()
