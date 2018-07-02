#ifndef TCP_COMM_H
#define TCP_COMM_H

#include <QObject>
#include <QTcpServer>

//#include <QNetworkSession>
class tcp_comm : public QObject
{
    Q_OBJECT
public:
    explicit tcp_comm(int port,QObject *parent = nullptr);
    ~tcp_comm();
    int writeDataViaTCP(QByteArray dataToWrite);
signals:
    void writeBackReceivedData(QByteArray receivedData);
public slots:
    void onNewConnection();
    void onReadyRead();
    void onStateCahnged(QAbstractSocket::SocketState state);
public:
    QTcpServer *tcpServer = nullptr;
    //QVector<QString> fortunes;
    //QNetworkSession *networkSession = nullptr;
    QTcpSocket *clientConnection=nullptr;
    int port=0;
    QByteArray data;
};

#endif // TCP_COMM_H
