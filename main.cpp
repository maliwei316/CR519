#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "QmlLanguage.h"
#include <QDebug>
#include <QList>
#include <QtSql>
#include <QDateTime>
#include "mbc_controller.h"
#include "dbh_controller.h"
#include <QObject>
#include "worker_modbus.h"
#include "tcp_comm.h"
#include "mainwindow.h"
#include <clsbarcode.h>
bool loggingEnable=true;
quint8 loggingLevel=99;

int main(int argc, char *argv[])
{
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QGuiApplication app(argc, argv);

    QApplication a(argc, argv);

    MainWindow w;

    qRegisterMetaType<QModbusDevice::State>();
    qRegisterMetaType<QModbusDevice::Error>();

    //QQmlApplicationEngine engine;
    //QmlLanguage qmlLanguage(app, engine);
    dbh_controller dbhc1;
    dbhc1.init("QSQLITE","realTimeDB",":memory:");
    dbh_controller dbhc2;
    QString filename="HistoryData"+QString::number(QDateTime::currentDateTime().date().year())+".sqlite3";
    dbhc2.init("QSQLITE","HistoryDataDB",filename);
    mbc_controller mbcc1;
    mbcc1.init("10.168.1.2",502,300,0,20);
    mbc_controller mbcc2;
    mbcc2.init("10.168.1.2",503,0,0,30);
    tcp_comm tcpcomm1;

    //qDebug()<<"mbcc1.thread:"<<mbcc1.thread();
//    engine.rootContext()->setContextProperty("mbcc1",&mbcc1);
//    engine.rootContext()->setContextProperty("mbcc2",&mbcc2);
//    engine.rootContext()->setContextProperty("tcpcomm1", &tcpcomm1);
//    engine.rootContext()->setContextProperty("qmlLanguage", &qmlLanguage);
//    engine.rootContext()->setContextProperty("dbhc1", &dbhc1);
//    engine.rootContext()->setContextProperty("dbhc2", &dbhc2);
//    engine.load(QUrl(QLatin1String("qrc:/main_100.qml")));
//    if (engine.rootObjects().isEmpty())
//        return -1;
   QObject::connect(&mbcc1,&mbc_controller::needbatchWriteDatabase,&dbhc1,&dbh_controller::addTaskToEventQueue_batchWriteDB);
   QObject::connect(&mbcc1,&mbc_controller::needWriteDatabase,&dbhc1,&dbh_controller::addTaskToEventQueue_writeDB);
   QObject::connect(&mbcc2,&mbc_controller::needWriteDatabase,&dbhc1,&dbh_controller::addTaskToEventQueue_writeDB);
   QObject::connect(&mbcc1,&mbc_controller::plcItemsChanged_mbc,&w,&MainWindow::OnPLCItemsChanged_Modbus);
   QObject::connect(&mbcc2,&mbc_controller::plcItemsChanged_mbc,&w,&MainWindow::OnPLCItemsChanged_Modbus);

   QObject::connect(&w,&MainWindow::checkModbusConnectionStatus,&mbcc1,&mbc_controller::needReportConnectionStatus);
   QObject::connect(&w,&MainWindow::checkModbusConnectionStatus,&mbcc2,&mbc_controller::needReportConnectionStatus);

   QObject::connect(&w,&MainWindow::sendDataToTCPCommObj,&tcpcomm1,&tcp_comm::receiveDataFromWindow);
   QObject::connect(&tcpcomm1,&tcp_comm::sendDataToWindow,&w,&MainWindow::receiveDataFromTCPCommObj);
   QObject::connect(&tcpcomm1,&tcp_comm::tcpCommConnectionStateChanged,&w,&MainWindow::OnTcpCommConnectionStateChanged);
   QObject::connect(&w,&MainWindow::checkTcpConnectionStatus,&tcpcomm1,&tcp_comm::reportConnectionStatus);
   //logging
   QObject::connect(&mbcc1,&mbc_controller::logRequest,&w,&MainWindow::execLogging);
   QObject::connect(&mbcc1,&mbc_controller::logRequest,&w,&MainWindow::execLogging);
   QObject::connect(&tcpcomm1,&tcp_comm::logRequest,&w,&MainWindow::execLogging);
   QObject::connect(&dbhc1,&dbh_controller::logRequest,&w,&MainWindow::execLogging);
   QObject::connect(&dbhc2,&dbh_controller::logRequest,&w,&MainWindow::execLogging);



   w.show();
    return a.exec();

 }
