#ifndef WORKER_MODBUS_H
#define WORKER_MODBUS_H

#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QObject>
#include <QEvent>
#include <QTimer>
#include <QVariant>
#include <QList>
#include "bitsoperation.h"

class worker_modbus : public QObject
{
    Q_OBJECT
public:
     explicit worker_modbus(QObject *parent = nullptr);
    ~worker_modbus();
    bool connectPLC();
    void setVarCounts(int DI_Var_count,int DO_Var_count,int HoldRegister_Var_count);
    void readPLCCommand(quint16 functionCode,quint16 startAddress, quint16 length);
    void setPLCCoilsTest();   
    void readPLCByInterval();
    void parseDataFromPLC(quint8 area,QVariantList addressList,QVariantList valueList);

signals:
    //void writeDatabaseRequired(QString sqlquery);
    //void batchWriteDataBaseRequired(QString prepareStr,QVariantList addressList,QVariantList valueList);
    void modbusStateChanged(QModbusDevice::State state);
    void modbusErrorOccured(QModbusDevice::Error error);
    void plcItemsChanged(QVariantList changedItems);
    void logRequest(QString logContents,quint16 logID,quint8 logLevel);
public slots:
    void onInit(QString IPAddr,int port,int DI_Var_count,int DO_Var_count,int HoldRegister_Var_count);
    void readReady2();
    void onStateChanged(QModbusDevice::State state);
    void onErrorOccurred(QModbusDevice::Error error);
    void writePLCCommand(quint16 functionCode, quint16 Address, const quint16 data,bool bitOperation,quint8 bitPos);
    void onCheckConnectionStatus();
public:

    QModbusTcpClient *maModbusTcpClient;
    QString IPAddr;
    int port;
    bool connectedFlag;
    bool DI_enable;
    bool DO_enable;
    bool HoldRegister_enable;
    bool timerEnableFlag;
    quint16 DI_Var_count;
    quint16 DO_Var_count;
    quint16 HoldRegister_Var_count;
    QTimer *timer;
    QMap<quint32,plcItem> plcItemMap;
    bool checkingConnectionStatus;
};


#endif // WORKER_MODBUS_H
