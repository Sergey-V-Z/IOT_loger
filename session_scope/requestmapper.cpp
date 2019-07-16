#include "requestmapper.h"

// Обявление переменных
HttpSessionStore* RequestMapper::sessionStore=nullptr;
StaticFileController * RequestMapper :: staticFileController = nullptr;
TemplateCache* RequestMapper::templateCache=nullptr;
Logger* RequestMapper::logger=nullptr;
CurrentPoint *RequestMapper::currentPoint=nullptr;
settingsControl *RequestMapper::settingsController=nullptr;
 QMap<QString, QByteArray> *RequestMapper::loginUsers=nullptr;
 QMap<QString, QByteArray> *RequestMapper::loginRoot=nullptr;

RequestMapper::RequestMapper(QSettings *settings, QSettings *loginBase, QObject* parent)
    : HttpRequestHandler(parent) {
    // Выделение памяти
    RequestMapper::currentPoint = new CurrentPoint (settings, parent); // глобальный объект класс текушей точки(для получения свежих данных)
    RequestMapper::settingsController = new settingsControl (parent); // Контроллер приема настроек
    eventController = new sseCtrl(parent);
    RequestMapper::loginUsers = new QMap<QString, QByteArray> ();
    RequestMapper::loginRoot = new QMap<QString, QByteArray> ();

    loginBase->beginGroup ("users");
    QStringList keysLogin = loginBase->allKeys ();
    for (int i = 0; i < keysLogin.size (); ++i) {
         RequestMapper::loginUsers->insert (keysLogin[i], loginBase->value (keysLogin[i]).toByteArray ());
    }
    keysLogin.clear ();
    loginBase->endGroup ();
    loginBase->beginGroup ("rootUser");
    keysLogin = loginBase->allKeys ();
    for (int i = 0; i < keysLogin.size (); ++i) {
         RequestMapper::loginRoot->insert (keysLogin[i], loginBase->value (keysLogin[i]).toByteArray ());
    }

}

RequestMapper::~RequestMapper()
{
    delete loginUsers;
    delete loginRoot;
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    HttpSession session=sessionStore->getSession(request,response,false);
    QString username=session.get("username").toString();
    logger->set("currentUser",username);

    //Перенаправление на login
    QByteArray sessionId=sessionStore->getSessionId(request,response);
    if (sessionId.isEmpty() && path!="/login") {
        qDebug("RequestMapper: redirect to login page");
        response.redirect("/login");
        return;
    }

    //Переход по ссылкам
    if(path == "/sourseset"){
        settingsController->service (request, response);
    }
    else if (path=="/list") {
        listDataController.service(request, response);
    }
    else if (path=="/login") {
        loginController.service(request, response);
    }
    else if (path=="/cookie") {
        cookieTestController.service(request, response);
    }
    else if (path.startsWith("/vf")) {
        currentPoint->service(request, response);
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
