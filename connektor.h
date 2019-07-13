#ifndef CONNEKTOR_H
#define CONNEKTOR_H

#include <QObject>

class Connektor : public QObject
{
    Q_OBJECT
public:
    explicit Connektor(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CONNEKTOR_H