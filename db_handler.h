#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#include <QObject>
#include <QtSql>
#include <QThread>
#include <QEvent>
#include <QVariant>
#include <QList>

class DB_Handler : public QObject
{
    Q_OBJECT
public:
    //DB_Handler(QString dbDriverName="QSQLITE", QString dbConnectionName="default",QString databaseName=":memory:",QObject *parent= nullptr);
    explicit DB_Handler(QObject *parent= nullptr);
    ~DB_Handler();
    bool execsql(QString sqlQuery);
    int writeDatabase(QString sqlQuery);
    int batchWriteDatabase(QString prepareStr, QVariantList addressList, QVariantList valueList);
    int writeDatabaseEventHandler(QEvent *e);
    int batchWriteDatabaseEventHandler(QEvent *e);
    int readDatabase(QString sqlquery);
    int readDatabaseEventHandler(QEvent *e);
    //void readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address);
signals:
    dataReadyDB2GUI(quint16 reqesterID,quint16 address, quint16 value);
public slots:
    void onInit(QString dbDriverName, QString dbConnectionName,QString databaseName);
    void onAddTaskToEventQueue_writeDB(QString sqlquery);
    void onAddTaskToEventQueue_batchWriteDB(QString prepareStr,QVariantList addressList,QVariantList valueList);
    void onAddtaskToEentQuene_readDB(quint16 requesterObjID,QString tableName,quint16 address);

public:
    QString dbDiverName;
    QString dbConnectionName;
    QString databaseName;
    QSqlDatabase database1;
    QSqlQuery q1;

private:
    void customEvent(QEvent *e); //该函数是父类QObject的虚函数


};

#endif // DB_HANDLER_H
