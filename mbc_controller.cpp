#include "mbc_controller.h"
#include "worker_modbus.h"
#include <QDebug>
mbc_controller::mbc_controller(QObject *parent) : QObject(parent)
{
    worker_modbus *worker = new worker_modbus;
    worker->moveToThread(&this->workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    //connect(this, &Controller::operate, worker, &Worker::doWork);
    //connect(worker, &Worker::resultReady, this, &Controller::handleResults);
    //connect(worker,&worker_modbus::modbusConnected,this,&mbc_controller::onModbusConnected);
    connect(worker,&worker_modbus::writeDatabaseRequired,this,&mbc_controller::needWriteDatabase);
    connect(worker,&worker_modbus::modbusStateChanged,this,&mbc_controller::onModbusStateChanged);
    connect(worker,&worker_modbus::modbusErrorOccured,this,&mbc_controller::onModbusErrorOccurred);
    connect(this,&mbc_controller::needInit,worker,&worker_modbus::onInit);
    workerThread.start();
}
void mbc_controller::init(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count)
{emit this->needInit(IPAddr,port,DI_Var_count,DO_Var_count,HoldRegister_Var_count);}
void mbc_controller::onModbusStateChanged(QModbusDevice::State state)
{

}
void mbc_controller::onModbusErrorOccurred(QModbusDevice::Error error)
{

}
mbc_controller::~mbc_controller()
{
    workerThread.quit();
    workerThread.wait();

}
void mbc_controller::readTest(quint16 functionCode,quint16 startAddress, quint16 length)
{
    //this->workerMBClient1->readPLCCommand(functionCode,startAddress,length);
}
void mbc_controller::setCoils()
{
    //this->workerMBClient1->setPLCCoilsTest();
}
void mbc_controller::resetCoils()
{
    //this->workerMBClient1->resetPLCCoilsTest();
}
void mbc_controller::checkConnectionState()
{
    //this->checkConnectionState();
}

void mbc_controller::connectPLC()
{
    //this->workerMBClient1->connectPLC();
}