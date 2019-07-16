#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QtDebug>
#include <QDataStream>
#include <QFile>
#include <QDateTime>
#include <QList>
#include"QSettings"
#include"QDir"
#include"source.h"
//#include "../session_scope/requestmapper.h"


//Класс "Сервера"
class server : public QObject{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    server(QSettings *settingSens, QSettings *settingOther, QObject *parent);
    ~server();
    QMap<QString, Source *> Sensors;

signals:

public slots:
    virtual void slotNewConnection();
    void disconnected();
    void errorServer();
    void readyRead();

private:
    QTcpServer* Tcp_server;
    QTcpSocket* m_Socket;
    QFile* my_file;
    QDateTime* Date;
    bool moreDebug = false;
    void sendToCliernt(QTcpSocket *pSocket, const QString &str);
    void sendToCliernt(QTcpSocket *pSocket, QByteArray &buffer);
};


#endif // SERVER_H
