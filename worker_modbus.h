#ifndef WORKER_MODBUS_H
#define WORKER_MODBUS_H

#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QObject>
#include <QEvent>
#include <QTimer>
#include <QVariant>
#include <QList>

class worker_modbus : public QObject
{
    Q_OBJECT
public:
     explicit worker_modbus(QObject *parent = nullptr);
    ~worker_modbus();
    bool connectPLC();
    //bool disconnectPLC();
    void setVarCounts(int DI_Var_count,int DO_Var_count,int HoldRegister_Var_count);
    Q_INVOKABLE void checkConnection(QString info);
    //void streamBytesToFile(const QByteArray& data,QString filename="log.dat");
    Q_INVOKABLE void readPLCCommand(quint16 functionCode,quint16 startAddress, quint16 length);
    Q_INVOKABLE void setPLCCoilsTest();
    Q_INVOKABLE void resetPLCCoilsTest();
    Q_INVOKABLE void readPLCtoRealtimeDB();

signals:
    //void resultReady(const QString tableName,const quint16 startAddress,const quint16 length);
    //void modbusConnected();
    //void inIdleStatus();
    void writeDatabaseRequired(QString sqlquery);
    void batchWriteDataBaseRequired(QString prepareStr,QVariantList addressList,QVariantList valueList);
    void modbusStateChanged(QModbusDevice::State state);
    void modbusErrorOccured(QModbusDevice::Error error);
public slots:
    void onInit(QString IPAddr,int port,int DI_Var_count,int DO_Var_count,int HoldRegister_Var_count);
    void readReady1();
    void readReady();
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    //void onTimeout();
public:

    QModbusTcpClient *maModbusTcpClient;
    QString IPAddr;
    int port;
    bool connectedFlag;
    bool DI_enable;
    bool DO_enable;
    bool HoldRegister_enable;
    bool busyFlag;
    quint16 DI_Var_count;
    quint16 DO_Var_count;
    quint16 HoldRegister_Var_count;
    QTimer *timer;


};


#endif // WORKER_MODBUS_H
