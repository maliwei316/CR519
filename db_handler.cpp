#include "db_handler.h"
#include <QEvent>
#include "myevent.h"
#include <QtSql>

extern bool loggingEnable;
extern quint8 loggingLevel;
//AddTaskToEventQueue-->customEvent-->customEventHandler-->read/write database
DB_Handler::DB_Handler(QObject *parent): QObject(parent)
{}
void DB_Handler::onInit(QString dbDriverName, QString dbConnectionName, QString databaseName)
{
    this->dbDiverName=dbDriverName;
    this->dbConnectionName=dbConnectionName;
    this->databaseName=databaseName;
    this->database1=QSqlDatabase::database();
    if (!database1.isValid())
    {
        database1 = QSqlDatabase::addDatabase(dbDriverName,dbConnectionName);
        if (!database1.isValid())
            qFatal("Cannot add database: %s,%s", qPrintable(databaseName),qPrintable(database1.lastError().text()));
    }
    database1.setDatabaseName(databaseName);
    //if database not open, then open it; if already opened, no need open it again, just use it
    if(!database1.isOpen())
    {
        if (!database1.open()) {
            qFatal("Cannot open database: %s,%s", qPrintable(databaseName),qPrintable(database1.lastError().text()));
        }
        else
        {
            qDebug()<<"Success to open database";
            this->q1=QSqlQuery(database1);
            if(0==QString::compare(databaseName,":memory:"))
            {

            }
            else
            {

                QString execStatement;
                //QString execStatement="CREATE TABLE IF NOT EXISTS history_USW_cycle_data(id integer primary key autoincrement,barcode TEXT, toolID integer ,result TEXT,testTime DATETIME)";
                execStatement="CREATE TABLE IF NOT EXISTS pointHistoryCycleData"
                              "(Barcode TEXT, pointNO integer ,pointName TEXT,pointWeldResult TEXT,amplitude integer,pressure integer,"
                              "weldTime integer,peakPower integer,weldEnergy integer,holdTime integer,CreatedTime TimeStamp NOT NULL DEFAULT (datetime('now','localtime')),primary key(Barcode,pointNO))";
                if (!q1.exec(execStatement)) {
                         qFatal("Failed to create table of name pointHistoryCycleData: %s", qPrintable(q1.lastError().text()));
                     }

                execStatement="CREATE TABLE IF NOT EXISTS partHistoryCycleData(CreatedTime TimeStamp NOT NULL DEFAULT (datetime('now','localtime')),partBarcode TEXT primary key, partWeldResult TEXT,toolID integer,toolName TEXT,location TEXT)";

                if (!q1.exec(execStatement)) {
                         qFatal("Failed to query database: %s", qPrintable(q1.lastError().text()));
                     }

            }
            qDebug()<<"tableNO in dataBase:"<<database1.tables().size();
              QStringList tableList= database1.tables();
              for(int i=0;i<tableList.size();i++)
              {
                  qDebug()<<QObject::tr("table[%1]:").arg(i)<<tableList.at(i);
              }


        }

    }

}

int DB_Handler::writeDatabase(QString sqlQuery)
{
   qDebug()<<QTime::currentTime()<<"writeDatabase executed";
    if(!this->q1.exec(sqlQuery))
   {
        qDebug()<<"failed to exec writeData statement";
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("failed to exec writeData statement,error:%1,SQL:%2")
                    .arg(q1.lastError().text()).arg(sqlQuery);
            emit this->logRequest(logcontents,200,0);
        }
        return 0;
   }

    //get total record counts in database table;
   //QSqlQuery q1("",database1);
   //q1.exec(QObject::tr("SELECT COUNT(*) AS N FROM %1").arg((tableName)));
   //q1.next();
   //int N=q1.value(0).toInt();
   //qDebug()<<QObject::tr("record counts in table  %1:").arg((tableName))<<N;
   return  1;
}
int DB_Handler::batchWriteDatabase(QString prepareStr, QVariantList addressList, QVariantList valueList)
{
   qDebug()<<QTime::currentTime()<<"Batch writeDatabase executed";
   q1.prepare(prepareStr);
   q1.addBindValue(addressList);
   q1.addBindValue(valueList);
   //q1.addbindValue(":address",static_cast<QVariantList>addressList);
   //q1.addbindValue(":value",static_cast<QVariantList>valueList);
   if(!this->q1.execBatch())
   {
        qDebug()<<"failed to exec batchWriteData statement";
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("failed to exec batchWriteData statement");
            emit this->logRequest(logcontents,200,0);
        }
        return 0;
   }
   q1.finish();
    //get total record counts in database table;
   //QSqlQuery q1("",database1);
   //q1.exec(QObject::tr("SELECT COUNT(*) AS N FROM %1").arg((tableName)));
   //q1.next();
   //int N=q1.value(0).toInt();
   //qDebug()<<QObject::tr("record counts in table  %1:").arg((tableName))<<N;
   return  1;
}
int DB_Handler::readDatabase(QString sqlquery)
{
    //qDebug()<<QTime::currentTime()<<"read Database executed";
    if(!q1.exec(sqlquery))
    {
        qDebug()<<q1.lastError();
        if(loggingEnable&&loggingLevel>0)
        {
            QString logcontents=tr("failed to exec batchWriteData statement,error:%1").arg(q1.lastError().text());
            emit this->logRequest(logcontents,200,0);
        }
        return -1;
    }
    else
    {
        //q1.next();
        //qDebug()<<QString("data from database1  Address:%1  Value:%2").arg(q1.value(0).toInt()).arg(q1.value(1).toInt());
        return 1;
    }
}

DB_Handler::~DB_Handler()
{
    if(this->database1.isOpen())
    {
       this->database1.close();
    }
    qDebug()<<"after closed,before remove database/connectionName";
    //this->database1.removeDatabase(this->dbConnectionName);

}
