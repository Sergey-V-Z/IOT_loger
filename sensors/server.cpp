#include "server.h"

server::server(QObject *parent) : QObject(parent)
{
}

server::server(DescriptionOfSensor *descriptSensors, QSettings *settingOther, QObject *parent) : QObject(parent){
    Tcp_server = new QTcpServer(this);
    my_file = new QFile(this);
    Date = new QDateTime();

    // Считываем ключи из настроек
    QStringList sensKey = descriptSensors->keys ();
    int sensInt = sensKey.length();
    // создание в цикле MAP  объектов сенсоров
    for (int i = 0; i < sensInt; ++i) {
            QString templat = descriptSensors->getTemplates (sensKey[i]); // получаем шаблон
            if(templat == "non"){
                qDebug () << "error in settings! key found but content is emty or not correckt";
                continue;
            }
            Sensors.insert(sensKey[i], new Source(sensKey[i], settingOther, Date ));
            Sensors.value(sensKey[i])->setTemplate(templat); // добовление шаблона в обьект

    }

    connect(Tcp_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    connect(Tcp_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(errorServer()));

    Tcp_server->listen(QHostAddress::Any, static_cast<quint16>(settingOther->value("port").toUInt()));
    qDebug()<<"Start server";

}

server::~server(){
    delete (Date);

    QStringList keyList = Sensors.keys();
    for (int i = 0; i < Sensors.size(); ++i) {
        delete Sensors.value(keyList[i]);
    }
}

// Новое соединение по порту источников
void server::slotNewConnection()
{
    //    qDebug()<<"Connection set";

    m_Socket = Tcp_server->nextPendingConnection(); // Получаем уазатель новый сокет

    if(m_Socket->state() == QTcpSocket::ConnectedState)
    {
        if(moreDebug){
            qDebug() << "Socket was connected at: " << m_Socket->peerAddress();
        }
    }

    // Связваем сигналы нового сокета с имеющимеся слотами
    connect(m_Socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(m_Socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    sendToCliernt(m_Socket,"Server Respons: Connected!"); // Отвечаем что соединение установленно
}

// Слот при отключении
void server::disconnected() {

    QTcpSocket* pClientSoket = dynamic_cast<QTcpSocket*>(sender()); // Полечаем указатель на отправителя
    if(moreDebug){
        qDebug() << "Socket Disconnected..."<< pClientSoket->peerAddress();
    }
    // Cleanup
    pClientSoket->deleteLater();
}

// Слот при ошибке в tcp сервере
void server::errorServer() {

    qDebug() << "Error: " << Tcp_server->errorString();
    Tcp_server->close();
}

// Слот приема данных
void server::readyRead() {

    //QRegExp reg("(2[0-3]|[0-1][0-9]):[0-5][0-9]:[0-5][0-9]");
   // int pos = 0;
    QTcpSocket* pClientSoket = dynamic_cast<QTcpSocket*>(sender());
    QString buffer = static_cast<QString>( pClientSoket->readAll());
    if(moreDebug){
        qDebug() << buffer;
    }
    Source *sens;
    QStringList listIN = buffer.split(';');
    if(listIN.count ()< 3){
        qWarning() << "Invailed formst message" << buffer;
        return;
    }
    QString firstParam = listIN[0], idString = listIN[1];
    //Удаление первых двух элементов что бы проше было сохранить настройки
//    listIN.removeAt(0);
//    listIN.removeAt(0);

    // проверка на присутстввие id и проверка на наличие такого ключа в сенсорах
    if(idString.contains("id=")) {
        if(Sensors.contains(idString)){
            sens = Sensors.value(idString);
        }else {
            qDebug() << "id not found in MAP sensors";
            return;
        }
    }else{
        qDebug() << "id in idString not found. invalide message";
        return;
    }

    //Если первый параметр это время то это данные датчиков
    if(firstParam == "date"){
        // если у нас есть данные об устройстве
        if(sens->isConnect()){
                           //sendToCliernt(pClientSoket,"wait=501 set?");
            if(sens->needToUpdateSet ()){
                QStringList tempNewSet = sens->getNewSettings ();
                int totalSet = tempNewSet.size ();
                QString sendSet;
                for (int i = 0; i < totalSet; ++i) {
                    sendSet += tempNewSet[i] + " ";
                }
                sendSet += "set?";
                sendToCliernt(pClientSoket, sendSet);
            }
        }
        //иначе если у нас нет данных об устройстве то запрашиваем
        else{
            sendToCliernt(pClientSoket,"set?");
        }
        sens->writeToFile(&buffer); // Пишем в файл
        // Иначе если этот параметр содержит stat значит то настройки
    }else if(firstParam.contains("stat")){
        sens->saveSettings(listIN);
    }else {
        qDebug() << "invalid message format";
        return;
    }
    //    sendToCliernt(pClientSoket,buffer);
}

void server::sendToCliernt(QTcpSocket* pSocket, const QString& str){
    QByteArray buffer;
    buffer.append(str);
    pSocket->write(buffer);
}

void server::sendToCliernt(QTcpSocket* pSocket, QByteArray& buffer){
    pSocket->write(buffer);
}
