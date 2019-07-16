#ifndef CONNEKTOR_H
#define CONNEKTOR_H

#include <QObject>
#include "session_scope/requestmapper.h"
#include"sensors/server.h"

class Connektor : public QObject
{
    Q_OBJECT
public:
    explicit Connektor(server* serv, RequestMapper* mapper, QObject *parent = nullptr);

public slots:

signals:

private:
    CurrentPoint* Point;
};

#endif // CONNEKTOR_H
