#ifndef COOKIETESTCONTROLLER_H
#define COOKIETESTCONTROLLER_H

#include "httprequesthandler.h"

using namespace stefanfrings;

class CookieTestController : public HttpRequestHandler
{
    Q_OBJECT

public:
    CookieTestController(QObject* parent=nullptr);
    void service(HttpRequest& request, HttpResponse& response);

};

#endif // COOKIETESTCONTROLLER_H
