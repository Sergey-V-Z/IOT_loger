/*
 * Класс датчиков
*/

#include "source.h"
//extern descriptSensors;
//Контструктор принимает указатели на обект файла для записи и указатель на обьект (времени)
Source::Source(QString id, QSettings *settings, QDateTime *date,QObject *parent) : QObject(parent)
{
    ID = id;
    logFolder = settings->value("logFolder").toString();
    QFileInfo configFile (settings->value("logFile").toString());
    Path = QFileInfo(configFile.absolutePath(),Path).absoluteFilePath()+ "/";
    log_file = new QFile ();
    Date = date;
    //настройка буферезации данных
    buffCount = settings->value ("bufferCount").toInt ();
}

Source::~Source()
{
    delete log_file;
}

QString Source::getID()
{
    return ID;
}

QString Source::getPath()
{
    return Path;
}

QStringList Source::getNewSettings()
{
    return newSettings;
}

void Source::clearNewSettings()
{
    newSettings.clear ();
}

QStringList Source::getActualSettings()
{
    return actualSettings;
}

//сохранение настроек из устройства
bool Source::saveSettings(QStringList settings)
{
    //Проверка сообщения на валидность
    QJsonObject objJSON, obj2JSON;
    int sizeList = settings.size ();
    if(sizeList != 3){
        qDebug() << "massage format fail.";
        return false;
    }
    if(settings[0] != "stat"){
        qDebug() << "massage parametr 1 fail.";
        return false;
    }
    if(settings[1 ]!= ID){
        qDebug() << "id not correct.";
        return false;
    }

    QStringList listSettings = settings[2].split (",");

    //проверка параметров и создание объекта JSON
    for (int i = 0; i < listSettings.size (); ++i) {
        QStringList tempList =  listSettings[i].split ("=");
        // проверка на правилность параметра
        if(tempList.size () <  2){
            qDebug() << "error parametr in settings";
            continue;
        }
        objJSON[tempList[0]] = tempList[1];
    }
    obj2JSON[settings[1]] = objJSON;

    emit settingsJSON (obj2JSON); // сохраняем в "виртуальном файле"
    actualSettings = listSettings; // сохраняем настройки в объекте
    Connect = true;
    NeedUpdateSet = false;

    return true;
}

// Установка настроек пришедших от пользователя
void Source::settingsSet(QString set)
{
    //переменные для перебора
    int i =0,a=0;
    QStringList listMes= set.split (";");
    //проверка на валидность, сохранение настроек и выстовление флага отправки
    if(listMes.size ()< 2){
        qDebug() << "error parametr in setting source";
        return;
    }
    if(listMes[0] !=  ID){
        qDebug() << "id not correct.";
        return;
    }

    QStringList listSet = listMes[1].split (",");

    // перебор настроек
    for ( i = 0; i < listSet.size (); ++i) {
        QStringList tempList =  listSet[i].split ("=");
        QStringList tempActual;

        for (a = 0;a< actualSettings.size (); ++a) {
            tempActual = actualSettings[a].split ("=");
            // проверка на правилность параметра
            if((tempList.size () <  2) && (tempActual.size () <  2) ){
                qDebug() << "error parametr in setting source";
                continue;
            }
            if(tempList[0] ==  tempActual[0]){ // если имена одинаковы
                if(tempList[1] !=  tempActual[1]){// и если разное содержимое
                    newSettings.append (listSet[i]); //то добовляем в новые настройки
                }
            }
            tempActual.clear ();
        }
        tempList.clear ();
    }

    // Проверка пришедших данных и если есть новые то сохроняем их для отправки
    NeedUpdateSet = true;
}

void Source::updateSet()
{
    Connect = false;
}

void Source::writeToFile(QString *buff)
{
//    bool isok = false;
//    qint64 len = 0;
    if(fileJSON){
        QJsonDocument tempJSON;
        QJsonObject tempObj = parsString(*buff);
        QJsonObject nameFile, curentPoint, emitJSON;

        QString strID = "id=" + QString :: number (tempObj.value ("id=").toInt ());
        QString FileDate = Date->currentDateTime().toString("dd.MM.yy")+ "_" + strID + ".dat";
        nameFile[strID] =  "/" +  logFolder + "/" + FileDate;

        tempJSON.setObject(tempObj); //Добовляем новый обьект из строки данных
        if(currentFileName == ""){currentFileName = Path +  FileDate;}

        // если изменилясь дата записываем буфер в файл и обновляем имя файла
        if(Path +  FileDate != currentFileName ){
            //записываем буфер в файл
            log_file->setFileName(currentFileName);
            writeBoofToFile(bufferData);
            currentFileName = Path +  FileDate;
            currentBufferCount = 0;
            bufferData.clear ();
            log_file->close();
        }else {
            log_file->setFileName(Path +  FileDate);

        }
        if(buffCount == currentBufferCount ){
            //записываем буфер в файл
            writeBoofToFile(bufferData);
            bufferData.clear ();
            currentBufferCount = 0;
            log_file->close();
        }else {
            //буфферезируем данные
            if(currentBufferCount == 0){
                bufferData +=   tempJSON .toJson(QJsonDocument::Compact);
            }else {
                bufferData +=  ",\n"+ tempJSON .toJson(QJsonDocument::Compact);
            }
            currentBufferCount++;
        }

        curentPoint[strID] = tempObj; // в файл пишем в качестве ключа id для экономии места на диске
        emitJSON[sensorName] = tempObj; // в виртуальный файл отпровляем  имя в качестве ключа
        emit setPoint(emitJSON, nameFile);


    }else {
        QString FileDate = Path+Date->currentDateTime().toString("dd.MM.yy")+".dat";
        log_file->setFileName(FileDate);

        // если файла нет создаем его иначе пишем в конец файла
        if(!log_file->exists(FileDate)){
            log_file->open(QIODevice::WriteOnly);
        }else log_file->open(QIODevice::Append);

        log_file->write(buff->toUtf8());
        log_file->write("\r\n");
        log_file->close();

    }

    // добавить запись JSON файла
}

// внутренная фунция для парсинга строки в JSON  формат
QJsonObject Source::parsString(QString raw)
{
    QJsonObject objJSON, objTemp;
    QStringList listParam = raw.split(";");
    QString firstParam = listParam[0];
    QString identifcation = listParam[1];
    // удаляем два первых обекта из листа
    listParam.removeAt(0);
    listParam.removeAt(0);

    if(firstParam == "date"){
        objTemp["Time"]= Date->currentDateTime ().toString ("HH:mm:ss");
    }else{
        objTemp["Time"]= "er:er:er";
    }
    /*
 * Здесь необходимо сделать проверку на количествро
 * шаблонов и пришедших значений если они разные заполнить шаблонны имен
*/
    for (int i = 0; i < listParam.length(); ++i) { //прокручиваем все пришедшие данные
        if(i>templatesName.length()){  //если данных болше чем шаблонов имен то заполняем простыми именами
            objTemp["nonTemp"+ QString::number(i)] = listParam[i];
        }else {
            objTemp[templatesName[i]] = listParam[i];
        }
    }
    objTemp["id="]= identifcation.split ("=")[1].toInt (); // выделяем из чтроки id=n номер n  и записываем его в объект
    //    objJSON["id"]=  identifcation.split ("=")[1]; // выделяем из чтроки id=n номер n  и записываем его в объект
    //    objJSON["data"]=objTemp;
    return objTemp;
}

int Source::getTemplateOfFile(QString file)
{
    // Чтение из файла шаблона и запись шаблона в свойства объекта
    QFile * templates =  new QFile ();
    QTextStream fileStream(templates);
    QStringList Templates;
    int ret = -1;
    templates->setFileName(file);
    if(templates->exists()){
        if(!templates->open(QIODevice::ReadOnly)){
            qDebug()<< "File not opened";
            return  ret;
        }
    }else {
        qDebug()<< "File no faund";
        return  ret;
    }

    Templates = fileStream.readAll().split("\n") ;
    templates->close();
    //сделать цыкл для поиска id  в шаблонах
    for (int i = 0; i < Templates.length(); ++i) {
        QStringList tempList = Templates[i].split(":");
        if(tempList[0] == ID){
            templatesName = tempList[1].split(",");
            ret = 0;
            break;
        }
    }
    return ret;
}

void Source::setTemplate(QString templates)
{
    QStringList tempList = templates.split(",");
    sensorName =  tempList[0];
    tempList.removeAt(0);
    templatesName = tempList;
}

bool Source::isConnect()
{
    return Connect;
}

bool Source::needToUpdateSet()
{
    return  NeedUpdateSet;
}

void Source::writeBoofToFile(QString data)
{
    if(!log_file->exists()){
        log_file->open(QIODevice::WriteOnly);
        log_file->write(data.toUtf8 ()); // пишем его в файл
    }else {
        log_file->open(QIODevice::Append | QIODevice::Text); //QIODevice::ReadWrite |
        if(log_file->size ()<= 1){// если файл пуст
            log_file->write(data.toUtf8 ());
        }else{
            log_file->write(",\n"); //
            log_file->write(data.toUtf8 ());
        }
    }

}
