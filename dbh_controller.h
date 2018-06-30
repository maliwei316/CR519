#ifndef DBH_CONTROLLER_H
#define DBH_CONTROLLER_H

#include <QObject>
#include "db_handler.h"
#include <QThread>
class dbh_controller : public QObject
{
    Q_OBJECT
public:
    explicit dbh_controller(QObject *parent = nullptr);
    ~dbh_controller();
    void readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address);
    Q_INVOKABLE int readDatabase(QString sqlquery);
    Q_INVOKABLE int writeDatabase(QString sqlQuery);
    void init(QString dbDriverName, QString dbConnectionName,QString databaseName);
    Q_INVOKABLE bool getBitsFromWord(quint16 wordVar, quint8 bitPos);
signals:
    void needInit(QString dbDriverName, QString dbConnectionName,QString databaseName);
    void dataReadyDBH2Controller2GUI(quint16 reqesterID,quint16 address, quint16 value);
    void addTaskToEventQueue_writeDB(QString sqlquery);
    void addTaskToEventQueue_readDB(quint16 requesterObjID,QString tableName,quint16 address);

public slots:
    void getValuefromDataBase(quint16 requesterObjID,QString tableName,quint16 address);
public:

    QThread workerThread;

};

#endif // DBH_CONTROLLER_H
