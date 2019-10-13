#include "currentpoint.h"
//#include "template.h"
//#include "requestmapper.h"

int CurrentPoint::countObj;
QJsonObject* CurrentPoint:: currentJSON;
QJsonObject* CurrentPoint:: currentSettings;
QJsonObject* CurrentPoint:: currentFileNames;

CurrentPoint::CurrentPoint(QSettings *settings, QObject *parent):HttpRequestHandler (parent)
{
    settingSensors = settings;
    logFolder = settingSensors->value("logFolder").toString();
    QString pathAliaseFile =  settingSensors->value("aliaseSet").toString();
    if(pathAliaseFile.isEmpty ()){
        qFatal("Error: setting \" aliaseSet\" in INI-file is empty");
    }
    aliaseFile = new QFile (pathAliaseFile);

    //Если виртуальные "файлы" не созданны создаем их
    if(currentJSON  != nullptr){
    }else{currentJSON = new QJsonObject (); }

    if(currentSettings != nullptr){
    }else{ currentSettings= new QJsonObject ();}

    if(currentFileNames != nullptr){
    }else{ currentFileNames= new QJsonObject (); }

    countObj++;
}

CurrentPoint::~CurrentPoint()
{
    delete currentJSON;
    delete currentSettings;
}

void CurrentPoint::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray path =  request.getPath();
    QJsonDocument tempDoc;
    if(path == "/vf/settings"){
        tempDoc.setObject (*currentSettings);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/aliaseSet") {
        QJsonObject aliaseJSON = QJsonDocument::fromJson ( request.getBody ()).object ();
        writeAliasInFile(aliaseJSON);
        response.setStatus (200, "OK");
         response.write("" ,true);
    }
    else if (path == "/vf/curentpoint") {
        tempDoc.setObject (*currentJSON);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/allFile") {
        // читаем все файлы в директории
        QStringList list = sortDateList (getFilesList ( "*.dat"));
        QJsonArray currentAllFiles;
        for (int i = 0; i < list.count (); ++i) {
            currentAllFiles.insert (i, "/" +  logFolder + "/" + list[i]);
        }
        tempDoc.setArray (currentAllFiles);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/listFile") {
        //  чтение файлов из папки и показ файлов с сегоднешней датой
        QDateTime date;
        QStringList list = getFilesList (date.currentDateTime().toString("dd.MM.yy")+ "*" + ".dat");
        QRegExp reg("id=\\d{0,3}");
        for (int i = 0; i < list.count (); ++i) {
            int pos = reg.indexIn (list[i]);
            if(pos != -1){
                QStringList find = reg.capturedTexts ();
                if(find.count ()> 1){continue;}
                currentFileNames->insert ( find[0], "/" + logFolder + "/" +list[i]);
            }else { continue;}
        }

        tempDoc.setObject (*currentFileNames);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/updateSet") {
        emit updateSetSensor ();
    }
    else{
        response.setHeader("Content-Type", "text/html; charset=UTF-8");
        response.write("<html><body>");
        response.write("Virtual file list: <br> <a href=\"/vf/settings \">Settings<a>");
        response.write(" <br> <a href=\"/vf/aliaseSet \">Set aliase for settings<a>");
        response.write(" <br> <a href=\"/vf/curentpoint \">Curent point<a>");
        response.write("<br> <a href=\"/vf/listFile \">List log Files<a> ");
        response.write("<br> <a href=\"/vf/allFile\">All Files in dir<a> ");
        response.write("<br> <a href=\"/vf/updateSet\">Update settings sensors<a> ");
        response.write("</body></html>",true);

    }

}

void CurrentPoint::setCurrenJSON(QJsonObject currentPoint, QJsonObject nameFile)
{
    // Добовляем в статические объекты данные если данные сушествуют они будут заменены новыми
    QJsonObject tempObj;

    QStringList listKey =  currentPoint.keys();
    int countKey = currentPoint.count ();
    for (int i = 0; i < countKey; ++i) {
        currentJSON->insert (listKey[i],currentPoint.value (listKey[i]));
    }

    QStringList list2Key =  nameFile.keys();
    int count2Key = nameFile.count ();
    for (int i = 0; i < count2Key; ++i) {
        currentFileNames->insert (list2Key[i],nameFile.value (list2Key[i]));
    }
    emit currentDataSignal (*currentJSON);

}

void CurrentPoint::setSettingsJSON(QJsonObject Settings)
{
    QStringList listKey =  Settings.keys();
    int countKey = Settings.count ();
    // прочитать алеасы из файла
    if(!aliaseFile->open (QIODevice::ReadWrite)){
        qFatal("Error: aliase file not opend");
    }
    QJsonObject jsonInFile = QJsonDocument::fromJson (aliaseFile->readAll ()).object ();

    if (jsonInFile.isEmpty ()) {
        // создать JSON обект с алеасами
        QJsonObject aliase = createAliase(Settings);
        jsonInFile = aliase;
        // Запись объекто в файл
        QJsonDocument doc(aliase);
        aliaseFile->write (doc.toJson ());
    } else {
        QJsonObject aliase;
        /* иначе если файл не пуст то считываем из него объект и ишем в нем нужный id если его нет то создаем
        JSON объект добвляем его в основной объест и затем пишем его в файл*/
        for (int i = 0; i < listKey.count (); ++i) {
            if (!jsonInFile.contains (listKey[i])) {
                // создать JSON обект с алеасами
                aliase = createAliase(Settings);
            }
            // Вставляем алеас в обект
            jsonInFile.insert (listKey[i], aliase[listKey[i]]);
        }
        QJsonDocument doc(jsonInFile);
        aliaseFile->remove ();
        if(!aliaseFile->open (QIODevice::ReadWrite)){
            qFatal("Error: aliase file not opend");
        }
        aliaseFile->write (doc.toJson ());
    }

    // Обект для сохранинеия
    QJsonObject finishObj ;
    for (int i = 0; i < countKey; ++i) {
        //соеденить два JSON объекта один из alias а другой из Settings и сохронить
        QJsonObject temp;
        temp.insert ("aliase", jsonInFile.value (listKey[i]));
        temp.insert ("settings", Settings.value (listKey[i]));
        finishObj.insert (listKey[i],temp);
        currentSettings->insert (listKey[i],finishObj.value (listKey[i]));
    }
    aliaseFile->close ();
}

/*
 * Ищит файлы по маске в заданной в настройках директории
*/
QStringList CurrentPoint::getFilesList(QString mask)
{
    QDir mDir;
    QString path;
    QFileInfo configFile (settingSensors->value("logFile").toString());
    path = QFileInfo(configFile.absolutePath(),path).absoluteFilePath();
    mDir.setPath(path);
    return  mDir.entryList(QStringList()<< mask,QDir::NoFilter,QDir::Time);;
}

/*
 *  Сортировка файлов по дате в имени файла
* @ list - принимает лист файлов на сортировку
*/
QStringList CurrentPoint::sortDateList(QStringList list )
{
    QMap<QDate, QString> map;
    foreach (QString str, list){
        QRegExp rx("[0-9]{2}.[0-9]{2}.[0-9]{2}");
        qint32 pos = rx.indexIn (str); // ищем в строке подстроку по регулярному выражению
        QString item;
        if(pos != -1){ // если строка найдена
            QStringList itemL = rx.capturedTexts (); // копируем найденые строки
            if(itemL.size () == 1){ // если строка одно
                item = itemL[0];
            }else {
                continue;
            }
        }else {
            qDebug() << "incorrect file name:"+ str;
        }
        QDate date = QDate::fromString (item,"dd.MM.yy" ); // переводим строку в дату
        map.insertMulti (date, str);
    }
    list = map.values();
    int n = list.size ();
    for (int i = 0; i != n /2; ++i) {
        list.swap (i, n-i-1);
    }
    return  list;

}

QJsonObject CurrentPoint::createAliase(QJsonObject settingsJSON)
{
    QStringList listKey =  settingsJSON.keys();
    int countKey = settingsJSON.count ();

    QJsonObject tmpObj;
    for (int i = 0; i < countKey; ++i) {
        QJsonObject tmpValObj = settingsJSON.value (listKey[i]).toObject ();
        QStringList keysValObj = tmpValObj.keys ();
        for (int a = 0; a < keysValObj.count (); ++a) {
            tmpValObj[keysValObj[a]] = "";
        }
        tmpObj.insert (listKey[i],tmpValObj);
    }
    return  tmpObj;
}

void CurrentPoint::writeAliasInFile(QJsonObject aliase)
{
    QStringList keys = aliase.keys ();
    // прочитать алеасы из файла
    if(!aliaseFile->open (QIODevice::ReadWrite)){
        qFatal("Error: aliase file not opend");
    }
    QJsonObject jsonInFile = QJsonDocument::fromJson (aliaseFile->readAll ()).object ();

    for (int i = 0; i < keys.count (); ++i) {
        if(jsonInFile.contains (keys[i])){
            jsonInFile.insert (keys[i], aliase.value (keys[i]));
        }
    }
    QJsonDocument doc(jsonInFile);
    aliaseFile->remove ();
    if(!aliaseFile->open (QIODevice::ReadWrite)){
        qFatal("Error: aliase file not opend");
    }
    aliaseFile->write (doc.toJson ());

}
