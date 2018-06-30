//#ifndef BITSOPERATION_H
//#define BITSOPERATION_H
//#include <stdio.h>
//#include <QList>
// //这里就是要把数字x的第n位（bit（n-1）位）置为1
// //1U就表示的是无符号的1，宏定义可以传参的
// #define SET_BIT(x,n)    (x | 1U<<(n-1))

// //这里就是要把数字x的第n位（bit（n-1）位）清零
// #define CLEAR_BIT(x,n)    (x & ~(1U<<(n-1)))

//这里就是要把数字x的第n到m位置为1(n是低位，m是高位)
//就是先把0取反就可以得到很多的1，然后左移就得到那么多个0，再取反就可以得到你想要的1的个数了
//最后左移位或就可以置1了
//#define SET_BITS(x,n,m)    (x | ~(~0U<<(m-n+1))<<(n-1))

//截取变量的部分连续位。(就是取出的意思)
//其实和上面那里是差不多的，后面那一大部分都是为了确定需要多少个1
//最后记得右移，为了得出那个数字
//#define GET_BIT(x,n,m)    (x & ~(~0U<<(m-n+1))<<(n-1)) >>(n-1)

//typedef union
//    {
//    struct
//                {
//                    unsigned char b0:1;
//                    unsigned char b1:1;
//                    unsigned char b2:1;
//                    unsigned char b3:1;
//                    unsigned char b4:1;
//                    unsigned char b5:1;
//                    unsigned char b6:1;
//                    unsigned char b7:1;


//                }bits;

//        unsigned char byteChar;
//    } bytebits;
//bytebits bybits1;
//bybits1.byteChar=0x02;
//if(bybits1.bits.b0)
//{


//}
//QList<bytebits> bybitsList1;

//    //qDebug("char:%c",by1.e);
//    //qDebug("bits:%s,%s,%s,%s\n",(by1.d.b0?"1":"0"),(by1.d.b1?"1":"0"),(by1.d.b2?"1":"0"),(by1.d.b3?"1":"0"));
//    //qDebug("bits:%s,%s,%s,%s\n",(by1.d.b4?"1":"0"),(by1.d.b5?"1":"0"),(by1.d.b6?"1":"0"),(by1.d.b7?"1":"0"));
//class Worker : public QObject
//{
//    Q_OBJECT

//public slots:
//    void doWork(const QString meter) {
//        // ...
//        emit resultReady(result);
//    }
//    void stopWork(){
//        //...
//    }
//signals:
//    void resultReady(const QString &result);
//};

//class Controller : public QObject
//{
//    Q_OBJECT

//    QThread workerThread;

//public:
//    Controller() {
//        Worker *worker = new Worker;
//        worker->moveToThread(&workerThread);
//        connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
//        connect(this, &Controller::operate, worker, &Worker::doWork);
//        connect(this, &Controller::kill, worker, &Worker::stopWork);
//        connect(worker, &Worker::resultReady, this, &Controller::handleResults);
//        workerThread.start();
//    }
//    ~Controller() {
//        workerThread.quit();
//        workerThread.wait();
//    }

//public slots:
//    void handleResults(const QString &);

//signals:
//    void operate(const QString &);
//    void kill();
//};
//class Worker : public QObject
//{
//    Q_OBJECT

//public slots:
//    void doWork(const QString &parameter) {
//        QString result;
//        /* ... here is the expensive or blocking operation ... */
//        emit resultReady(result);
//    }

//signals:
//    void resultReady(const QString &result);
//};

//class Controller : public QObject
//{
//    Q_OBJECT
//    QThread workerThread;
//public:
//    Controller() {
//        Worker *worker = new Worker;
//        worker->moveToThread(&workerThread);
//        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
//        connect(this, &Controller::operate, worker, &Worker::doWork);
//        connect(worker, &Worker::resultReady, this, &Controller::handleResults);
//        workerThread.start();
//    }
//    ~Controller() {
//        workerThread.quit();
//        workerThread.wait();
//    }
//public slots:
//    void handleResults(const QString &);
//signals:
//    void operate(const QString &);
//};
//#endif // BITSOPERATION_H
