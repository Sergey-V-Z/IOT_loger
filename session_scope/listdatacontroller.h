#ifndef LISTDATACONTROLLER_H
#define LISTDATACONTROLLER_H
#include<QList>
#include<QString>
#include "httprequesthandler.h"

using namespace stefanfrings;

class ListDataController : public HttpRequestHandler
{
    Q_OBJECT
public:
    ListDataController(QObject* parent=nullptr);
    void service(HttpRequest& request, HttpResponse& response);
private:
    QList<QString> list;
};

#endif // LISTDATACONTROLLER_H
