#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#include <QObject>
#include <QtSql>
#include <QThread>
//#include <QEvent>
#include <QVariant>
#include <QList>

class DB_Handler : public QObject
{
    Q_OBJECT
public:
    //DB_Handler(QString dbDriverName="QSQLITE", QString dbConnectionName="default",QString databaseName=":memory:",QObject *parent= nullptr);
    explicit DB_Handler(QObject *parent= nullptr);
    ~DB_Handler();
    //bool execsql(QString sqlQuery);
    int writeDatabase(QString sqlQuery);
    int batchWriteDatabase(QString prepareStr, QVariantList addressList, QVariantList valueList);
    int readDatabase(QString sqlquery);
    void onInit(QString dbDriverName, QString dbConnectionName,QString databaseName);

signals:
    void logRequest(QString logContents,quint16 logID,quint8 logLevel);
public slots:


public:
    QString dbDiverName;
    QString dbConnectionName;
    QString databaseName;
    QSqlDatabase database1;
    QSqlQuery q1;

private:

};

#endif // DB_HANDLER_H
