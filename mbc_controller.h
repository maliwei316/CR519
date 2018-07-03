#ifndef MBC_CONTROLLER_H
#define MBC_CONTROLLER_H
#include "worker_modbus.h"
#include <QObject>
#include <QThread>
#include <QVariant>
#include <QList>
class mbc_controller : public QObject
{
    Q_OBJECT

    //Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)


public:
    explicit mbc_controller(QObject *parent = nullptr);
    ~mbc_controller();
    Q_INVOKABLE void readTest(quint16 functionCode=1,quint16 startAddress=0, quint16 length=1);
    Q_INVOKABLE void setCoils();
    Q_INVOKABLE void resetCoils();
    Q_INVOKABLE void checkConnectionState();
    //Q_INVOKABLE void readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address);
    void connectPLC();
    void init(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count);


signals:

    void needInit(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count);
    void needWriteDatabase(QString sqlquery);
    void needbatchWriteDatabase(QString prepareStr,QVariantList addressList,QVariantList valueList);
public slots:
    void onModbusStateChanged(QModbusDevice::State state);
    void onModbusErrorOccurred(QModbusDevice::Error error);

public:
    //worker_modbus *workerMBClient1;
    QThread workerThread;
};

#endif // MBC_CONTROLLER_H
