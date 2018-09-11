#include "dbh_controller.h"
#include "db_handler.h"

extern bool loggingEnable;
extern quint8 loggingLevel;
typedef union
    {
    struct
                {
                    unsigned char b8:1;
                    unsigned char b9:1;
                    unsigned char b10:1;
                    unsigned char b11:1;
                    unsigned char b12:1;
                    unsigned char b13:1;
                    unsigned char b14:1;
                    unsigned char b15:1;
                    unsigned char b0:1;
                    unsigned char b1:1;
                    unsigned char b2:1;
                    unsigned char b3:1;
                    unsigned char b4:1;
                    unsigned char b5:1;
                    unsigned char b6:1;
                    unsigned char b7:1;
                }bitsVar;
        quint16 wordVar;
    } wordBits;
dbh_controller::dbh_controller(QObject *parent) : QObject(parent)
{
    DB_Handler* worker=new DB_Handler;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    //connect(this, &Controller::operate, worker, &Worker::doWork);
    //connect(worker, &Worker::resultReady, this, &Controller::handleResults);
    connect(worker,&DB_Handler::dataReadyDB2GUI,this,&dbh_controller::dataReadyDBH2Controller2GUI);

    connect(this,&dbh_controller::addTaskToEventQueue_readDB,worker,&DB_Handler::onAddtaskToEentQuene_readDB);
    connect(this,&dbh_controller::addTaskToEventQueue_writeDB,worker,&DB_Handler::onAddTaskToEventQueue_writeDB);
    connect(this,&dbh_controller::addTaskToEventQueue_batchWriteDB,worker,&DB_Handler::onAddTaskToEventQueue_batchWriteDB);
    connect(this,&dbh_controller::needInit,worker,&DB_Handler::onInit);
    //logging
    connect(worker,&DB_Handler::logRequest,this,&dbh_controller::logRequest);
    workerThread.start();
}
void dbh_controller::init(QString dbDriverName, QString dbConnectionName,QString databaseName)
{emit this->needInit(dbDriverName,dbConnectionName,databaseName); }
dbh_controller::~dbh_controller()
{
    workerThread.quit();
    workerThread.wait();

}
void dbh_controller::readRealtimeDataFromDatabase(quint16 requesterObjID,QString tableName,quint16 address)
{
    //this->dbh1->readRealtimeDataFromDatabase(requesterObjID,tableName,address);
}
int dbh_controller::readDatabase(QString sqlquery)
{
    //this->dbh1->readDatabase(sqlquery);
}
int dbh_controller::writeDatabase(QString sqlQuery)
{
    //this->dbh1->writeDatabase(sqlQuery);
}
void dbh_controller::getValuefromDataBase(quint16 requesterObjID,QString tableName,quint16 address)
{emit this->addTaskToEventQueue_readDB(requesterObjID,tableName,address);}
bool dbh_controller::getBitsFromWord(quint16 wordVar,quint8 bitPos)
{
    qDebug()<<"data from HMI(wordVar):"<<wordVar;
    bool returnValue=false;
    wordBits wb1;
    wb1.wordVar=wordVar;
//    bool bitArray[16];
//    bitArray[0]=wb1.bitsVar.b0;
//    bitArray[1]=wb1.bitsVar.b1;
//    bitArray[2]=wb1.bitsVar.b2;
//    bitArray[3]=wb1.bitsVar.b3;
//    bitArray[4]=wb1.bitsVar.b4;
//    bitArray[5]=wb1.bitsVar.b5;
//    bitArray[6]=wb1.bitsVar.b6;
//    bitArray[7]=wb1.bitsVar.b7;
//    bitArray[8]=wb1.bitsVar.b8;
//    bitArray[9]=wb1.bitsVar.b9;
//    bitArray[10]=wb1.bitsVar.b10;
//    bitArray[11]=wb1.bitsVar.b11;
//    bitArray[12]=wb1.bitsVar.b12;
//    bitArray[13]=wb1.bitsVar.b13;
//    bitArray[14]=wb1.bitsVar.b14;
//    bitArray[15]=wb1.bitsVar.b15;
//    for(int i=0;i<16;i++)
//    {
//        qDebug()<<tr("bit[%1]:%2").arg(i).arg(bitArray[i]);
//    }
    switch (bitPos) {
    case 0:
        returnValue=wb1.bitsVar.b0;
        break;
    case 1:
        returnValue=wb1.bitsVar.b1;
        break;
    case 2:
        returnValue=wb1.bitsVar.b2;
        break;
    case 3:
        returnValue=wb1.bitsVar.b3;
        break;
    case 4:
        returnValue=wb1.bitsVar.b4;
        break;
    case 5:
        returnValue=wb1.bitsVar.b5;
        break;
    case 6:
        returnValue=wb1.bitsVar.b6;
        break;
    case 7:
        returnValue=wb1.bitsVar.b7;
        break;
    case 8:
        returnValue=wb1.bitsVar.b8;
        break;
    case 9:
        returnValue=wb1.bitsVar.b9;
        break;
    case 10:
        returnValue=wb1.bitsVar.b10;
        break;
    case 11:
        returnValue=wb1.bitsVar.b11;
        break;
    case 12:
        returnValue=wb1.bitsVar.b12;
        break;
    case 13:
        returnValue=wb1.bitsVar.b13;
        break;
    case 14:
        returnValue=wb1.bitsVar.b14;
        break;
    case 15:
        returnValue=wb1.bitsVar.b15;
        break;

    default:
        returnValue=0;
        break;
    }
    return returnValue;
}

