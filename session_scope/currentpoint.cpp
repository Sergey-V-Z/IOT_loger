#include "currentpoint.h"
//#include "template.h"
//#include "requestmapper.h"

int CurrentPoint::countObj;
QJsonObject* CurrentPoint:: currentJSON;
QJsonObject* CurrentPoint:: currentSettings;
QJsonObject* CurrentPoint:: currentFileNames;
QJsonArray* CurrentPoint:: currentAllFiles;

CurrentPoint::CurrentPoint(QSettings *settings, QObject *parent):HttpRequestHandler (parent)
{
    settingSensors = settings;
    logFolder = settingSensors->value("logFolder").toString();

    //Если виртуальные "файлы" не созданны создаем их
    if(currentJSON  != nullptr){
    }else{currentJSON = new QJsonObject (); }

    if(currentSettings != nullptr){
    }else{ currentSettings= new QJsonObject ();}

    if(currentFileNames != nullptr){
    }else{ currentFileNames= new QJsonObject (); }

    if(currentAllFiles != nullptr){
    }else{ currentAllFiles= new QJsonArray (); }

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
    else if (path == "/vf/curentpoint") {
        tempDoc.setObject (*currentJSON);
        response.write(tempDoc.toJson() ,true);
    }
    else if (path == "/vf/allFile") {
        // читаем все файлы в директории
        QStringList list = sortDateList (getFilesList ( "*.dat"));
        for (int i = 0; i < list.count (); ++i) {
            currentAllFiles->insert (i, "/" +  logFolder + "/" + list[i]);
        }
        tempDoc.setArray (*currentAllFiles);
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
        response.write(" <br> <a href=\"/vf/curentpoint \">Curent point<a>");
        response.write("<br> <a href=\"/vf/listFile \">List log Files<a> ");
        response.write("<br> <a href=\"/vf/allFile\">All Files in dir<a> ");
        response.write("<br> <a href=\"/vf/updateSet\">Update settings sensors<a> ");

        //        response.write("<br> <a href=\"/vf/settings \">settings<a> ");
        response.write("</body></html>",true);

    }
    //    QByteArray str = "not point, ";
    //    str += QByteArray::number(countObj);


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
    for (int i = 0; i < countKey; ++i) {
        currentSettings->insert (listKey[i],Settings.value (listKey[i]));
    }
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
