#include "mbc_controller.h"
#include "worker_modbus.h"
#include <QDebug>
#include <QVariant>

extern bool loggingEnable;
extern quint8 loggingLevel;

mbc_controller::mbc_controller(QObject *parent) : QObject(parent)
{
    worker_modbus *worker = new worker_modbus;
    worker->moveToThread(&this->workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    //connect(this, &Controller::operate, worker, &Worker::doWork);
    //connect(worker, &Worker::resultReady, this, &Controller::handleResults);

    connect(worker,&worker_modbus::plcItemsChanged,this,&mbc_controller::plcItemsChanged_mbc);
    connect(this,&mbc_controller::needInit,worker,&worker_modbus::onInit);
    connect(this,&mbc_controller::needWritePLC,worker,&worker_modbus::writePLCCommand);
    connect(this,&mbc_controller::needReportConnectionStatus,worker,&worker_modbus::onCheckConnectionStatus);
    //logging
    connect(worker,&worker_modbus::logRequest,this,&mbc_controller::logRequest);
    workerThread.start();
}
void mbc_controller::init(QString IPAddr, int port, int DI_Var_count, int DO_Var_count, int HoldRegister_Var_count)
{emit this->needInit(IPAddr,port,DI_Var_count,DO_Var_count,HoldRegister_Var_count);}

mbc_controller::~mbc_controller()
{
    workerThread.quit();
    workerThread.wait();

}
