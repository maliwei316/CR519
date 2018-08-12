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
    explicit tcp_comm(int portReceive, int portSend,QObject *parent = nullptr);
    ~tcp_comm();
    int writeDataViaTCP(QByteArray dataToWrite);
    void parseDataFromPLC(const QByteArray& dataToParse);
signals:

    void sendDataToWindow(QByteArray dataToWindow);
    void tcpCommConnectionStateChanged(QAbstractSocket::SocketState state,quint8 ConnectionID);

public slots:

    void onReadyRead();
    void onStateChanged(QAbstractSocket::SocketState state);
    //void onTokenStatusChanged(bool status);
    void prepareDataToPLC(QByteArray newData);
    void receiveDataFromWindow(QByteArray dataFromWindow);

    void reportConnectionStatus();
public:
    QTcpServer *tcpServer = nullptr;
    //QVector<QString> fortunes;
    //QNetworkSession *networkSession = nullptr;
    QTcpSocket *clientConnection_receive=nullptr;
    QTcpSocket *clientConnection_send=nullptr;
    int port=0;
    QByteArray receivedDataFromPLC;
    //QQueue<QByteArray> dataToSend;
    QByteArray dataToSend;



};

#endif // TCP_COMM_H
