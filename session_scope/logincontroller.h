#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include "httprequesthandler.h"
#include <QMultiMap>
#include <QtSql>

using namespace stefanfrings;

class LoginController : public HttpRequestHandler {
    Q_OBJECT
public:
    LoginController(QSettings * loginSet, QObject* parent=nullptr);
    void service(HttpRequest& request, HttpResponse& response);
private:
    QSettings * loginSet;
    QString pathBase;
    QString nameTable;

};

#endif // LOGINCONTROLLER_H
