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
#include "datacontroller.h"
#include "descriptionofsensor.h"

using namespace stefanfrings;

class RequestMapper : public HttpRequestHandler {
    Q_OBJECT

public:
    RequestMapper(DescriptionOfSensor *descriptSensors, QSettings* settings,  QSettings*loginBase, QObject* parent=nullptr);
    ~RequestMapper();
    void service(HttpRequest& request, HttpResponse& response);

    static HttpSessionStore * sessionStore;
    static StaticFileController * staticFileController;
    static TemplateCache* templateCache;
    static Logger* logger;
    static CurrentPoint *currentPoint;
    static settingsControl *settingsController;
    sseCtrl *eventController;

private:
    bool logInUser;
    ListDataController listDataController;
    LoginController *loginController;
    CookieTestController cookieTestController;
    DataTemplateController dataTemplateController;
    DescriptionOfSensor *descriptSensors;
    DataController dataJSON;
};

#endif // REQUESTMAPPER_H
