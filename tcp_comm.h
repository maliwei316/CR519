#ifndef TCP_COMM_H
#define TCP_COMM_H

#include <QObject>
#include <QTcpServer>

//#include <QNetworkSession>

class tcp_comm : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int PageNO READ getCurrentPageNO WRITE setCurrentPageNO NOTIFY pageNOChanged)

public:
    explicit tcp_comm(int portReceive, int portSend,QObject *parent = nullptr);
    ~tcp_comm();
     int writeDataViaTCP(QByteArray dataToWrite);
    int getCurrentPageNO();
    void setCurrentPageNO(int newPageNO);
    void parseReceivedData(const QByteArray& dataToParse);
signals:
    //void writeBackReceivedData(QByteArray receivedData);
    void pageNOChanged(int newPageNO);
    //void tokenStatusChanged(bool tokenAtPC);
    void sendDataToWindow(QByteArray dataToWindow);

public slots:

    void onReadyRead();
    void onStateChanged(QAbstractSocket::SocketState state);
    //void onTokenStatusChanged(bool status);
    void prepareDataToPLC(QByteArray newData);
    void receiveDataFromWindow(QByteArray dataFromWindow);

public:
    QTcpServer *tcpServer = nullptr;
    //QVector<QString> fortunes;
    //QNetworkSession *networkSession = nullptr;
    QTcpSocket *clientConnection_receive=nullptr;
    QTcpSocket *clientConnection_send=nullptr;
    int port=0;
    QByteArray data;
    QByteArray dataToSend;
    int pageNO;
    bool isHavingToken;

};

#endif // TCP_COMM_H
