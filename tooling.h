#ifndef TOOLING_H
#define TOOLING_H
#include "bitsoperation.h"
#include <QObject>

class tooling : public QObject
{
    Q_OBJECT
public:
    explicit tooling(QObject *parent = nullptr);

signals:

public slots:

public:

    quint8 toolID;
    QString toolName;
    quint8 weldPointQty;
    QList<weldPoint> weldPointList;
};

#endif // TOOLING_H
