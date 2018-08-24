#include "mbc_controller.h"
#include "worker_modbus.h"
#include <QDebug>
#include <QVariant>
mbc_controller::mbc_controller(QObject *parent) : QObject(parent)
{
    worker_modbus *worker = new worker_modbus;
    worker->moveToThread(&this->workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    //connect(this, &Controller::operate, worker, &Worker::doWork);
    //connect(worker, &Worker::resultReady, this, &Controller::handleResults);
    //connect(worker,&worker_modbus::modbusConnected,this,&mbc_controller::onModbusConnected);
    connect(worker,&worker_modbus::writeDatabaseRequired,this,&mbc_controller::needWriteDatabase);//signal transfer by mbc_controller
    connect(worker,&worker_modbus::batchWriteDataBaseRequired,this,&mbc_controller::needbatchWriteDatabase);//signal transfer by mbc_controller
    connect(worker,&worker_modbus::modbusStateChanged,this,&mbc_controller::onModbusStateChanged);
    connect(worker,&worker_modbus::modbusErrorOccured,this,&mbc_controller::onModbusErrorOccurred);
    connect(worker,&worker_modbus::plcItemsChanged,this,&mbc_controller::plcItemsChanged_mbc);
    connect(this,&mbc_controller::needInit,worker,&worker_modbus::onInit);
    connect(this,&mbc_controller::needWritePLC,worker,&worker_modbus::writePLCCommand);
    connect(this,&mbc_controller::needReportConnectionStatus,worker,&worker_modbus::onCheckConnectionStatus);
    workerThread.start();
}
void mbc_controller::init(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count)
{emit this->needInit(IPAddr,port,DI_Var_count,DO_Var_count,HoldRegister_Var_count);}
//void mbc_controller::writePLC(quint16 functionCode, quint16 Address, const quint16 data)
//{
    //emit this->needWritePLC(functionCode,Address,data);
//}
void mbc_controller::onModbusStateChanged(QModbusDevice::State state)
{
    qDebug()<<"mbc_controller says, modbus state changed,current state:"<<state;
}
void mbc_controller::onModbusErrorOccurred(QModbusDevice::Error error)
{
    qDebug()<<"mbc controller says, modbus error occured,error:"<<error;
}
mbc_controller::~mbc_controller()
{
    workerThread.quit();
    workerThread.wait();

}
