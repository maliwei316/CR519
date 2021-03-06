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

public:
    explicit mbc_controller(QObject *parent = nullptr);
    ~mbc_controller();
    void init(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count);

signals:
    void logRequest(QString logContents,quint16 logID,quint8 logLevel);
    void needInit(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count);
    //void needWriteDatabase(QString sqlquery);
    //void needbatchWriteDatabase(QString prepareStr,QVariantList addressList,QVariantList valueList);
    void needWritePLC(quint16 functionCode, quint16 Address, const quint16 data,bool bitOperation,quint8 bitPos);
    void plcItemsChanged_mbc(QVariantList changedItems);
    void needReportConnectionStatus();
public slots:


public:
    //worker_modbus *workerMBClient1;
    QThread workerThread;
};

#endif // MBC_CONTROLLER_H
