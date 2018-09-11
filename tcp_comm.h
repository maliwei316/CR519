#ifndef TCP_COMM_H
#define TCP_COMM_H

#include <QObject>
#include <QTcpServer>
#include<QQueue>
//#include <QNetworkSession>
#include <QTimer>
class tcp_comm : public QObject
{
    Q_OBJECT


public:
    explicit tcp_comm(QString remoteIP="10.168.1.2", int portReceive_local=2000, int portSend_local=2001, int portReceive_remote=2001, int portSend_remote=2000, QObject *parent = nullptr);
    ~tcp_comm();
    int writeDataViaTCP(QByteArray dataToWrite);
    void parseDataFromPLC(const QByteArray& dataToParse);
signals:

    void sendDataToWindow(QByteArray dataToWindow);
    void tcpCommConnectionStateChanged(QAbstractSocket::SocketState state,quint8 ConnectionID);
    void logRequest(QString logContents,quint16 logID,quint8 logLevel);

public slots:

    void onReadyRead();
    void onStateChanged(QAbstractSocket::SocketState state);
    //void onTokenStatusChanged(bool status);
    void prepareDataToPLC(QByteArray newData);
    void receiveDataFromWindow(QByteArray dataFromWindow);

    void reportConnectionStatus();
public:
    QTcpServer *tcpServer = nullptr;

    QTcpSocket *clientConnection_receive=nullptr;
    QTcpSocket *clientConnection_send=nullptr;
    int port=0;
    QByteArray receivedDataFromPLC;

    QByteArray dataToSend;



};

#endif // TCP_COMM_H
