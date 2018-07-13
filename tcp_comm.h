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
     Q_INVOKABLE void writePLCTestViaTCP();
    int getCurrentPageNO();
    void setCurrentPageNO(int newPageNO);
    void parseReceivedData(const QByteArray& dataToParse);
signals:
    void writeBackReceivedData(QByteArray receivedData);
    void pageNOChanged(int newPageNO);
public slots:
    //void onNewConnection();
    void onReadyRead();
    void onStateChanged(QAbstractSocket::SocketState state);
public:
    QTcpServer *tcpServer = nullptr;
    //QVector<QString> fortunes;
    //QNetworkSession *networkSession = nullptr;
    QTcpSocket *clientConnection_receive=nullptr;
    QTcpSocket *clientConnection_send=nullptr;
    int port=0;
    QByteArray data;
    int pageNO;
    bool isHavingToken;
};

#endif // TCP_COMM_H
