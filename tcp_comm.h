#ifndef TCP_COMM_H
#define TCP_COMM_H

#include <QObject>

class tcp_comm : public QObject
{
    Q_OBJECT
public:
    explicit tcp_comm(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TCP_COMM_H