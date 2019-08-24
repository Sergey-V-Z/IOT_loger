#ifndef DATATEMPLATECONTROLLER_H
#define DATATEMPLATECONTROLLER_H

#include "httprequesthandler.h"

using namespace stefanfrings;

class DataTemplateController: public HttpRequestHandler {
    Q_OBJECT
public:
    DataTemplateController(QObject* parent=nullptr);
    void service(HttpRequest& request, HttpResponse& response);
private:

};

#endif // DATATEMPLATECONTROLLER_H
