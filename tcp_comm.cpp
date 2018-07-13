#include "tcp_comm.h"
#include <QDebug>
#include <QTcpSocket>
#include "bitsoperation.h"
#include <QEventLoop>
#include <QTimer>
#include <QTime>
#pragma pack(1)

tcp_comm::tcp_comm(int portReceive, int portSend,QObject *parent) : QObject(parent)
{
      this->tcpServer=new QTcpServer(this);
     if(tcpServer->isListening())
         tcpServer->close();
      if (!tcpServer->listen(QHostAddress::AnyIPv4,portReceive)) {
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
        if (!tcpServer->listen(QHostAddress::AnyIPv4,portSend)) {
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
    if(this->clientConnection_receive->bytesAvailable()>=4)
    {
        QEventLoop eventloop;

        QTimer::singleShot(20, &eventloop, SLOT(quit()));
        eventloop.exec();
        data=this->clientConnection_receive->readAll();

        qDebug()<<"data.size()"<<data.size();
       for(int i=0;i<data.count();i++)
       qDebug()<<tr("data from PLC, at[%1]:%2,data[%3]:%4,peer port:%5").arg(i).arg(QString::number(data.at(i),16)).arg(i).arg(data[i]).arg(this->clientConnection_receive->peerPort());
       //QByteArray receivedBytearray(data);
       QByteArrayList receivedByteArrayList;

       if(data[2]==0x01)
       {
           this->isHavingToken=true;
       }
       else
       {
          this->isHavingToken=false;
       }
       if(data.size()>4)
       {
           int subDataLength;
           int i=4;
           while(i<data.size())
           {
             subDataLength=data[i]*256+data[i+1];
             receivedByteArrayList.append(data.mid(i,subDataLength));
             i+=subDataLength;
           }
           for(int i=0;i<receivedByteArrayList.size();i++)
           {
             this->parseReceivedData(receivedByteArrayList.at(i));
           }

       }
       emit writeBackReceivedData(data);
    }

}
void tcp_comm::parseReceivedData(const QByteArray& dataToParse)
{
    int dataToPase_size=dataToParse.size();
    if(dataToPase_size<6)
    {
       qDebug()<<"subData size <6";
        return;
    }
    if(dataToPase_size!=dataToParse.at(0)*256+dataToParse.at(1))
    {
       qDebug()<<"subData size not equal with length in byte array";
        return;
    }

    quint16 command=dataToParse.at(2)*256+dataToParse.at(3);
    qDebug()<<"command NO:"<<command;
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

    case 100://change HMI page NO
        int receivedPageNO;
        receivedPageNO=dataLoad.at(0)*256+dataLoad.at(1);
        qDebug()<<"received pageNO:"<<receivedPageNO;
        qDebug()<<"this.PageNO(before change):"<<this->pageNO;
        this->setCurrentPageNO(receivedPageNO);
        qDebug()<<"this.pageNO(after change):"<<this->pageNO;
        break;

    default:
        qDebug()<<"Command not match , no assigned hanlder";
        break;
    }

}

void tcp_comm::writePLCTestViaTCP()
{
   QByteArray ba;
   //ba<<0x00<<0x04<<0x02<<0x33;
   ba.clear();
   ba.append(char(0x01));
   ba.append(char(0x02));
   ba.append(char(0x03));
   ba.append(char(0x04));
   this->writeDataViaTCP(ba);
}
int tcp_comm::writeDataViaTCP(QByteArray dataToWrite)
{
    //qDebug()<<"writeDataViaTCP executed,data to write:"<<dataToWrite;
    int wroteCount=0;

    //dataToWrite[dataToWrite.count()-1]=(dataToWrite.at(dataToWrite.count()-1))?0x00:0xff;
   dataToWrite[2]=(dataToWrite[2]+1)%2;
   //dataToWrite[1]=4;
   qDebug()<<"writeDataViaTCP executed,data to write:"<<dataToWrite;
    wroteCount=this->clientConnection_send->write(dataToWrite);
    qDebug()<<"writeDataViaTCP executed,wrote count:"<<wroteCount<<"peer port:"<<this->clientConnection_send->peerPort();

    return wroteCount;
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
       qDebug()<<"pageNO changed, new page NO"<<this->pageNO;

   }
}

void tcp_comm::onStateChanged(QAbstractSocket::SocketState state)
{
   qDebug()<<"QTcpSocket state changed, current state:"<<state;
}
tcp_comm::~tcp_comm()
{
    this->tcpServer->deleteLater();

}

#pragma pack()
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
