#include <QTime>
#include "logincontroller.h"
#include "requestmapper.h"

LoginController::LoginController(QSettings *loginSet, QObject* parent)
    :HttpRequestHandler(parent) {
    pathBase = loginSet->value ("pathBase").toString ();
    nameTable = loginSet->value ("nameTable").toString ();
    // empty
}

void LoginController::service(HttpRequest &request, HttpResponse &response) {
    HttpSession session=RequestMapper::sessionStore->getSession(request,response,true);
    QByteArray username=request.getParameter("username");
    QByteArray password=request.getParameter("password");
    QByteArray setUser=request.getParameter("setuser");
    bool ok;

    QMultiMap<QByteArray,QByteArray> ParameterMap  = request.getParameterMap();

    qDebug("username=%s",username.constData());
    qDebug("password=%s",password.constData());

    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    // Работа с базой данных
    if(!setUser.isEmpty () and request.getMethod ()== "POST"){
        bool registrUser = session.contains("username");
        // Регистрация пользователя в базе данных
        if(setUser == "registr" and registrUser and username != ""){ // Регистрация пользователя
            bool userRegistered = false;
            QSqlDatabase usersBase = QSqlDatabase::addDatabase("QSQLITE");
            usersBase.setDatabaseName(pathBase);
            ok = usersBase.open ();
            QSqlQuery usersTable;
            //читаем таблицу и проверяем есть ли пользователь в ней
            usersTable.prepare ("SELECT * FROM  "+nameTable);
            usersTable.exec ();

            //перебор пользователей
            if(usersTable.isActive ()){
                usersTable.next ();
                QString name = usersTable.value(0).toString();
                for (int i = 0; name != ""; ++i) {
                    if(name == username){
                        userRegistered = true;
                        break;
                    }
                    usersTable.next ();
                    name = usersTable.value(0).toString();
                }
            }
            if(userRegistered){
                response.setStatus(409,"Conflict");
                response.write("{\"respons\" : \"Registered\"}",true);
            }else {
                usersTable.prepare ("INSERT INTO "+nameTable+" (name, password)"
                                    "VALUES (:User, :Pass);");
                usersTable.bindValue (":User",username);
                usersTable.bindValue (":Pass", password);
                usersTable.exec ();
                if(usersTable.isActive ()){
                    response.setStatus(200,"OK");
                    response.write("{\"respons\" : \"hawe registered\"}",true);
                }else{
                    response.setStatus(400,"ERR");
                    response.write("{\"respons\" : \"ERR\"}",true);
                }

            }
            usersBase.close ();
            usersTable.clear ();
        }
        //Удаление ползователя из базы
        else if (setUser == "delet" and registrUser ) { // Удаление пользователя
            QSqlDatabase usersBase = QSqlDatabase::addDatabase("QSQLITE");
            usersBase.setDatabaseName(pathBase);
            ok = usersBase.open ();
            QSqlQuery usersTable;
             ok = usersTable.prepare ("SELECT * FROM  "+nameTable+" "
                                   "WHERE name=:User;");
             usersTable.bindValue (":User",username);
             ok = usersTable.exec ();
             usersTable.next ();
             QString tempPass = usersTable.value (1).toString ();
            if ( tempPass == password) {
                ok = usersTable.prepare ("DELETE FROM "+nameTable+" "
                                         "WHERE name=:User;");
                usersTable.bindValue (":User",username);
                ok = usersTable.exec ();
                if(usersTable.isActive ()){
                    response.setStatus(200,"OK");
                    response.write("{\"respons\" : \"deleted\"}",true);
                }else{
                    response.setStatus(400,"ERR");
                    response.write("{\"respons\" : \"base access error\"}",true);
                }

            } else {
                response.setStatus(400,"ERR");
                response.write("{\"respons\" : \"pass error\"}",true);

            }
            usersBase.close ();
            usersTable.clear ();
        }
        //Получение списка ползователей
        else if (setUser == "users" and registrUser) { // Список всех пользователей
            QJsonObject users;
            //            QStringList listKey = RequestMapper::userStor->keys ();
            QSqlDatabase usersBase = QSqlDatabase::addDatabase("QSQLITE");
            usersBase.setDatabaseName(pathBase);
            ok = usersBase.open ();
            QSqlQuery usersTable;
            usersTable.prepare ("SELECT * FROM "+nameTable);
            usersTable.exec ();
            //             QSqlRecord rec = usersTable.record (); // возврошвет теущую строку из таблицы
            if(usersTable.isActive ()){
                usersTable.next ();
                QString name = usersTable.value(0).toString();
                for (; name != ""; ) {
                    users.insert (name,"");
                    usersTable.next ();
                    name = usersTable.value(0).toString();
                }
                QJsonDocument userJSON(users);
                response.setStatus(200,"OK");
                response.write(userJSON.toJson (QJsonDocument::Compact),true);
            }else{
                response.setStatus(400,"ERR");
                response.write("{\"respons\" : \"ERR\"}",true);
            }
            usersBase.close ();
            usersTable.clear ();


            //            for (int i = 0; i < listKey.size (); ++i) {
            //                //                QString val = RequestMapper::userStor->value (listKey[i]);
            //                users.insert (listKey[i],"");
            //            }
        }
        else{
            if(!registrUser){
                response.setStatus(401 ,"Unauthorized");
                response.redirect("/login.html");
            }
            else {
                response.setStatus(400 ,"Bad Request");
                response.write("{\"respons\" : \"bead parametr\"}",true);
            }
        }

    }
    // завершение сесии (logout)
    else if (!setUser.isEmpty () and request.getMethod ()== "GET") {
        if (setUser == "logout" and session.contains("username")) {
            session.remove ("username");
            response.setStatus(200,"OK");
            response.redirect("/index.html");
        }
    }
    //Запрос страницы
    else if(username.isEmpty () and password.isEmpty () and setUser.isEmpty ()){
        if(request.getMethod () == "GET" || request.getMethod () == "get"){ // если метод GET значит у нас просят страницу
            RequestMapper::staticFileController->service (request,response);
        }else {
            response.setStatus(406,"Not Acceptable");
            //                    response.write("No, that was wrong!",true);
            qCritical("Login failed!");// запись в лог файл
            response.redirect("/login.html");
        }
    }
    // вход ползователя (login)
    else {
        //если сесия содержит имя ползователя то все ок
        if (session.contains("username")) {
            QByteArray username=session.get("username").toByteArray();
            QTime logintime=session.get("logintime").toTime();
            //            response.write("You are already logged in.<br>");
            //            response.write("Your name is: "+username+"<br>");
            //            response.write("Your Time out: "+QTime:: (session.getLastAccess())+"<br>");
            response.setStatus(201,"Accepted");
            response.write("You logged in at: "+logintime.toString("HH:mm:ss").toLatin1(),true);
        }
        else {
            // если в сесии нет имени и пришло имя и пароль то проверяем на соответствие
            bool loginOK=false;

            //ишем ползователя в записях
            QSqlDatabase usersBase = QSqlDatabase::addDatabase("QSQLITE");
            usersBase.setDatabaseName(pathBase);
            ok = usersBase.open ();
            QSqlQuery usersTable;
            ok = usersTable.prepare ("SELECT * FROM "+nameTable);
            ok = usersTable.exec ();
            //             QSqlRecord rec = usersTable.record (); // возврошвет теущую строку из таблицы
            if(usersTable.isActive ()){
                usersTable.next ();
                QString name = usersTable.value(0).toString();
                QString pass = usersTable.value(1).toString();
                for (; name != ""; ) {
                    if(name == username and  pass == password){
                        loginOK = true;
                        break;
                    }
                    usersTable.next ();
                    name = usersTable.value(0).toString();
                    pass = usersTable.value(1).toString();
                }
            }else{
                response.setStatus(400,"ERR");
                response.write("{\"respons\" : \"base error\"}",true);
            }
            usersBase.close ();
            usersTable.clear ();
            // если нашли пользователя в записях
            if (loginOK) {
                session.set("username",username);
                session.set("logintime",QTime::currentTime());
                response.setStatus(200,"OK");
                response.write ("");
                //                response.redirect("/index.html");
            }
        }
    }

}

