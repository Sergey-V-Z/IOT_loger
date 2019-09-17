QT -= gui
QT += network sql

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        session_scope/listdatacontroller.cpp \
        session_scope/requestmapper.cpp \
        session_scope/logincontroller.cpp \
        session_scope/cookietestcontroller.cpp \
        session_scope/datatemplatecontroller.cpp \
        sensors/server.cpp \
        sensors/source.cpp \
    session_scope/currentpoint.cpp \
    connektor.cpp \
    session_scope/settingscontrol.cpp \
    session_scope/ssectrl.cpp \
    descriptionofsensor.cpp \
    session_scope/datacontroller.cpp \
    sensors/collectorfiles.cpp

OTHER_FILES += etc/webapp.ini

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(../QtWebApp/httpserver/httpserver.pri)
include(../QtWebApp/templateengine/templateengine.pri)
include(../QtWebApp/logging/logging.pri)
include(../QtWebApp/qtservice/qtservice.pri)

HEADERS += \
    session_scope/listdatacontroller.h \
    session_scope/requestmapper.h \
    session_scope/logincontroller.h \
    session_scope/cookietestcontroller.h \
    session_scope/datatemplatecontroller.h\
    sensors/server.h \
    sensors/source.h \
    session_scope/currentpoint.h \
    connektor.h \
    session_scope/settingscontrol.h \
    session_scope/ssectrl.h \
    descriptionofsensor.h \
    session_scope/datacontroller.h \
    sensors/collectorfiles.h

INSTALLS        = target
target.path     = /home/pi/qtbin/webServer/webHomeServer
