#include "db_handler.h"
#include <QEvent>
#include "myevent.h"
#include <QtSql>

extern bool loggingEnable;
extern quint8 loggingLevel;

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
    if (!database1.open()) {
        qFatal("Cannot open database: %s,%s", qPrintable(databaseName),qPrintable(database1.lastError().text()));

    }
    else
    {
        qDebug()<<"Success to open database";
        this->q1=QSqlQuery("",database1);
        if(0==QString::compare(databaseName,":memory:"))
        {
            if (!q1.exec("create table IF NOT EXISTS PLC_DI(address integer primary key ,value integer)")) {
                     qFatal("Failed to query database line39: %s", qPrintable(q1.lastError().text()));
                 }
            if (!q1.exec("create table IF NOT EXISTS PLC_DO(address integer primary key,value integer)")) {
                     qFatal("Failed to query database line42: %s", qPrintable(q1.lastError().text()));
                 }

            if (!q1.exec("create table IF NOT EXISTS PLC_M(address integer primary key,value integer)")) {
                     qFatal("Failed to query database line46: %s", qPrintable(q1.lastError().text()));
                 }
            if (!q1.exec("create table IF NOT EXISTS PLC_DB(address integer primary key,value integer)")) {
                     qFatal("Failed to query database line49: %s", qPrintable(q1.lastError().text()));
                 }
    //        if (!q1.exec("create table IF NOT EXISTS PLC_DI(address integer primary key ,value integer)")) {
    //                 qFatal("Failed to query database line39: %s", qPrintable(q1.lastError().text()));
    //             }

        }
        else
        {

            QString execStatement;
            //QString execStatement="CREATE TABLE IF NOT EXISTS history_USW_cycle_data(id integer primary key autoincrement,barcode TEXT, toolID integer ,result TEXT,testTime DATETIME)";
            execStatement="CREATE TABLE IF NOT EXISTS history_USW_cycle_data(id integer primary key autoincrement,barcode TEXT, toolID integer ,result TEXT,testTime DATETIME)";

            if (!q1.exec(execStatement)) {
                     qFatal("Failed to query database: %s", qPrintable(q1.lastError().text()));
                 }

            execStatement="CREATE TABLE IF NOT EXISTS detail_history_USW_cycle_data(id integer primary key autoincrement,barcode TEXT, pointNO integer ,weldEnergy integer,weldPeakPower integer,weldTime integer)";

            if (!q1.exec(execStatement)) {
                     qFatal("Failed to query database: %s", qPrintable(q1.lastError().text()));
                 }
            execStatement="CREATE TABLE IF NOT EXISTS pointsNmaeMapping(id integer primary key autoincrement,toolID integer CHECK(toolID>0 AND toolID<32),pointNO integer CHECK(pointNO>0 AND pointNO<17),pointName TEXT, enableStatus integer)";
            if (!q1.exec(execStatement)) {
                     qFatal("Failed to query database: %s", qPrintable(q1.lastError().text()));
                 }

        }
        qDebug()<<"tableNO in dataBase:"<<database1.tables().size();
//          QStringList tableList= database2.tables();
//          for(int i=0;i<tableList.size();i++)
//          {
//              qDebug()<<QObject::tr("table[%1]:").arg(i)<<tableList.at(i);
//          }


    }
}
void DB_Handler::customEvent(QEvent *e)
{
    switch (e->type())
    {
    case 5001://捕获消息,5001 表示来自word_Modbus的写数据库消息
        writeDatabaseEventHandler(e);
        break;
    case 5002:
        readDatabaseEventHandler(e);
        break;
    case 5003:
        batchWriteDatabaseEventHandler(e);
        break;
    default:
        break;
    }

}
int DB_Handler::writeDatabaseEventHandler(QEvent *e)
{
    //qDebug()<<QTime::currentTime()<<"writeDatabaseEventHandler executed"<<"Thread:"<<QThread::currentThread();
    myEvent_writeDB *customEvent = static_cast<myEvent_writeDB *>(e);
    int recordsCount=writeDatabase(customEvent->sqlquery);
    return recordsCount;

}
int DB_Handler::batchWriteDatabaseEventHandler(QEvent *e)
{
    //qDebug()<<QTime::currentTime()<<"BatchWriteDatabaseEventHandler executed";
    myEvent_batchWriteDB *customEvent = static_cast<myEvent_batchWriteDB *>(e);
    int recordsCount=batchWriteDatabase(customEvent->prepareStr,customEvent->addressList,customEvent->valueList);
    return recordsCount;

}
int DB_Handler::writeDatabase(QString sqlQuery)
{
   qDebug()<<QTime::currentTime()<<"writeDatabase executed";
    if(!this->q1.exec(sqlQuery))
   {
        qDebug()<<"failed to exec writeData statement";
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

int DB_Handler::readDatabaseEventHandler(QEvent *e)
{
   //qDebug()<<QTime::currentTime()<<"readDatabaseEventHandler executed";
   myEvent_readDB *customEvent=static_cast<myEvent_readDB*>(e);
   QString sqlquery=QObject::tr("select address,value from %1 where address=%2" ).arg(customEvent->tableName)
           .arg(customEvent->address);
   quint16 value=this->readDatabase(sqlquery);
   emit dataReadyDB2GUI(customEvent->requestObjID,customEvent->address,value);
   qDebug()<<QTime::currentTime()<<tr("data to HMI:customEvent->requestObjID:%1,customEvent->address:%2,value:%3")
             .arg(customEvent->requestObjID).arg(customEvent->address).arg(value);
   return value;
}


int DB_Handler::readDatabase(QString sqlquery)
{
    //qDebug()<<QTime::currentTime()<<"read Database executed";
    if(!q1.exec(sqlquery))
    {
        qDebug()<<q1.lastError();
        return -1;
    }
    else
    {
        q1.next();
        //qDebug()<<QString("data from database1  Address:%1  Value:%2").arg(q1.value(0).toInt()).arg(q1.value(1).toInt());
        return q1.value(1).toInt();
    }
}
//void DB_Handler::readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address)
//{
//    //post read query to DB handler,operate realTime DB
//    QCoreApplication::postEvent(this,new myEvent_readDB((QEvent::Type)5002,requesterObjID,tableName,address));

//}
void DB_Handler::onAddTaskToEventQueue_writeDB(QString sqlquery)
{
    qDebug()<<QTime::currentTime()<<"onAddTaskToEventQueue_writeDB executed"<<"Thread:"<<QThread::currentThread();
    QCoreApplication::postEvent(this,new myEvent_writeDB((QEvent::Type)5001,sqlquery));
}
void DB_Handler::onAddTaskToEventQueue_batchWriteDB(QString prepareStr,QVariantList addressList,QVariantList valueList)
{
    qDebug()<<QTime::currentTime()<<"onAddTaskToEventQueue_batchWriteDB executed";
    //QCoreApplication::postEvent(this,new myEvent_batchWriteDB((QEvent::Type)5003,prepareStr,addressList,valueList));
    this->batchWriteDatabase(prepareStr,addressList,valueList);
}
void DB_Handler::onAddtaskToEentQuene_readDB(quint16 requesterObjID,QString tableName,quint16 address)
{
    //post read query to DB handler,operate realTime DB
    qDebug()<<QTime::currentTime()<<"onAddTaskToEventQueue_readDB executed";
    QCoreApplication::postEvent(this,new myEvent_readDB((QEvent::Type)5002,requesterObjID,tableName,address));
}
DB_Handler::~DB_Handler()
{

}
