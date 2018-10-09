#ifndef DBH_CONTROLLER_H
#define DBH_CONTROLLER_H

#include <QObject>
#include "db_handler.h"
#include <QThread>
#include <QVariant>
#include <QList>
class dbh_controller : public QObject
{
    Q_OBJECT
public:
    explicit dbh_controller(QObject *parent = nullptr);
    ~dbh_controller();
    void init(QString dbDriverName, QString dbConnectionName,QString databaseName);
    bool getBitsFromWord(quint16 wordVar, quint8 bitPos);
signals:
    void needInit(QString dbDriverName, QString dbConnectionName,QString databaseName);
    //void dataReadyDBH2Controller2GUI(quint16 reqesterID,quint16 address, quint16 value);
    void addTaskToEventQueue_writeDB(QString sqlquery);
    void addTaskToEventQueue_batchWriteDB(QString prepareStr,QVariantList addressList,QVariantList valueList);
    void addTaskToEventQueue_readDB(QString sqlquery);
    void logRequest(QString logContents,quint16 logID,quint8 logLevel);

public slots:

public:

    QThread workerThread;

};

#endif // DBH_CONTROLLER_H
