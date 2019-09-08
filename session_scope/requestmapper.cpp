#include "requestmapper.h"

// Обявление переменных
HttpSessionStore* RequestMapper::sessionStore=nullptr;
StaticFileController * RequestMapper :: staticFileController = nullptr;
TemplateCache* RequestMapper::templateCache=nullptr;
Logger* RequestMapper::logger=nullptr;
CurrentPoint *RequestMapper::currentPoint=nullptr;
settingsControl *RequestMapper::settingsController=nullptr;

RequestMapper::RequestMapper(DescriptionOfSensor *descriptSensors, QSettings *settings, QSettings *loginSet, QObject* parent)
    : HttpRequestHandler(parent) {
    // Выделение памяти
     loginController = new LoginController (loginSet, parent);
    RequestMapper::currentPoint = new CurrentPoint (settings, parent); // глобальный объект класс текушей точки(для получения свежих данных)
    RequestMapper::settingsController = new settingsControl (parent); // Контроллер приема настроек
    eventController = new sseCtrl(parent);
//    RequestMapper::userStor = new QMap<QString, QByteArray> ();
    this->descriptSensors = descriptSensors;
    //loginController.setLoginBase(loginSet);
}

RequestMapper::~RequestMapper()
{
//    delete userStor;
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    HttpSession session=sessionStore->getSession(request,response,false);
    QString username=session.get("username").toString();
    logger->set("currentUser",username);

    //Перенаправление на login
    QByteArray sessionId=sessionStore->getSessionId(request,response);
    QString referrer = request.getHeader ("referer");
    if ((sessionId.isEmpty() || username.isEmpty ()) &&  path != "/login.html" &&  !referrer.contains ("/login.html")) {
        qDebug("RequestMapper: redirect to login page");
        response.redirect("/login.html");
        return;
    }

    //Переход по ссылкам
    if(path == "/sourseset"){
        settingsController->service (request, response);
    }
    else if (path=="/list") {
        listDataController.service(request, response);
    }
    else if (path=="/login.html") {
        loginController->service(request, response);
    }
    else if (path=="/cookie") {
        cookieTestController.service(request, response);
    }
    else if (path.startsWith("/vf")) {
        currentPoint->service(request, response);
    }
    else if (path.startsWith("/data")) {
        dataJSON.service(request, response, descriptSensors);
    }
    else if (path.startsWith("/sse")) {
        eventController->service(request, response);
    }
    else if (path.startsWith ("/menu")) {
        dataTemplateController.service(request, response);
    }
    else if (path.startsWith("/files") || path.startsWith ("/")) {
        staticFileController->service(request,response);
    }

    else {
        response.setStatus(404,"Not found");
        response.write("The URL is wrong, no such document.",true);
    }

    qDebug("RequestMapper: finished request");
    logger->clear(true,true);
}
