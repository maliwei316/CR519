#include "tcp_comm.h"
#include <QDebug>
#include <QTcpSocket>
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
    data=this->clientConnection_receive->readAll();
    for(int i=0;i<data.count();i++)
        qDebug()<<tr("data from PLC, at[%1]:%2,peer port:%3").arg(i).arg(QString::number(data.at(i),16)).arg(this->clientConnection_receive->peerPort());
    emit writeBackReceivedData(data);
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
