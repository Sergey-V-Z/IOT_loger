#include <QTime>
#include "logincontroller.h"
#include "requestmapper.h"
#include <QMultiMap>

LoginController::LoginController(QObject* parent)
    :HttpRequestHandler(parent) {
    // empty
}

void LoginController::service(HttpRequest &request, HttpResponse &response) {
    HttpSession session=RequestMapper::sessionStore->getSession(request,response,true);
    QByteArray username=request.getParameter("username");
    QByteArray password=request.getParameter("password");
    QByteArray setUser=request.getParameter("setuser");

    QMultiMap<QByteArray,QByteArray> ParameterMap  = request.getParameterMap();

    qDebug("username=%s",username.constData());
    qDebug("password=%s",password.constData());

    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
//    response.write("<html><body>");


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
        if (username=="test" and password=="hello") {
            response.setStatus(200,"OK");
            response.write("Yes, correct",true);
            session.set("username",username);
            session.set("logintime",QTime::currentTime());
        }
        else {
//            response.write("<form method='POST' action='/login'>");
            if(request.getMethod () == "GET" || request.getMethod () == "get"){ // если метод GET значит у нас просят страницу
                RequestMapper::staticFileController->service (request,response);
            }else {
                response.setStatus(406,"Not Acceptable");
                response.write("No, that was wrong!",true);
                qCritical("Login failed!");// запись в лог файл

            }

//            response.write("Please log in:<br>");
//            response.write("Name:  <input type='text' name='username'><br>");
//            response.write("Password: <input type='password' name='password'><br>");
//            response.write("<input type='submit'>");
//            response.write("</form");

        }
    }

//    response.write("",true);
}
