#include "tcp_comm.h"
#include <QDebug>
#include <QTcpSocket>
tcp_comm::tcp_comm(int port, QObject *parent) : QObject(parent)
{
      this->tcpServer=new QTcpServer(this);
      if (!tcpServer->listen(QHostAddress::Any,port)) {
          qDebug()<<tr("Unable to start the server: %1.")
                    .arg(tcpServer->errorString());

    }
      else
      {
          qDebug()<<"listening?"<<tcpServer->isListening();
          connect(tcpServer, &QTcpServer::newConnection, this, &tcp_comm::onNewConnection);


      }

}
void tcp_comm::onNewConnection()
{

    //QByteArray block;
    //QDataStream out(&block, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);

   // out << fortunes[QRandomGenerator::global()->bounded(fortunes.size())];

    this->clientConnection = tcpServer->nextPendingConnection();
    qDebug()<<"new connection detected,peer port"<<this->clientConnection->peerPort();
    connect(clientConnection,&QTcpSocket::stateChanged,this,&tcp_comm::onStateCahnged);
    connect(clientConnection, &QAbstractSocket::disconnected,clientConnection, &QObject::deleteLater);
    //clientConnection->write(block);
    //clientConnection->disconnectFromHost();
    connect(clientConnection,&QAbstractSocket::readyRead,this,&tcp_comm::onReadyRead);
}
void tcp_comm::onReadyRead()
{
    data=this->clientConnection->readAll();
    for(int i=0;i<data.count();i++)
        qDebug()<<tr("data from PLC, at[%1]:%2,peer port:%3").arg(i).arg(QString::number(data.at(i),16)).arg(this->clientConnection->peerPort());
    emit writeBackReceivedData(data);
}void tcp_comm::onStateCahnged(QAbstractSocket::SocketState state)
{
   qDebug()<<"QTcpSocket state changed, current state:"<<state;
}

int tcp_comm::writeDataViaTCP(QByteArray dataToWrite)
{
    qDebug()<<"writeDataViaTCP executed,data to write:"<<dataToWrite;
    int wroteCount=0;

    dataToWrite[dataToWrite.count()-1]=(dataToWrite.at(dataToWrite.count()-1))?0x00:0xff;

    wroteCount=this->clientConnection->write(dataToWrite);
    qDebug()<<"writeDataViaTCP executed,wrote count:"<<wroteCount<<"peer port:"<<this->clientConnection->peerPort();
    return wroteCount;
}
tcp_comm::~tcp_comm()
{
    this->tcpServer->deleteLater();

}
