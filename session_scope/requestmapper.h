#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "httprequesthandler.h"
#include "listdatacontroller.h"
#include "logincontroller.h"
#include "httpsessionstore.h"
#include "cookietestcontroller.h"
#include "staticfilecontroller.h"
#include "templatecache.h"
#include "datatemplatecontroller.h"
#include "logger.h"
#include "currentpoint.h"
#include "settingscontrol.h"
#include "ssectrl.h"

using namespace stefanfrings;

class RequestMapper : public HttpRequestHandler {
    Q_OBJECT

public:
    RequestMapper( QSettings* settings,  QSettings*loginBase, QObject* parent=nullptr);
    ~RequestMapper();
    void service(HttpRequest& request, HttpResponse& response);

    static HttpSessionStore * sessionStore;
    static StaticFileController * staticFileController;
    static TemplateCache* templateCache;
    static Logger* logger;
    static CurrentPoint *currentPoint;
    static settingsControl *settingsController;
    static QMap<QString, QByteArray> *loginUsers;
    static QMap<QString, QByteArray> *loginRoot;
    sseCtrl *eventController;

private:
    ListDataController listDataController;
    LoginController loginController;
    CookieTestController cookieTestController;
    DataTemplateController dataTemplateController;
};

#endif // REQUESTMAPPER_H
