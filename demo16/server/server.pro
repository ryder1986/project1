#-------------------------------------------------
#
# Project created by QtCreator 2018-03-20T22:20:24
#
#-------------------------------------------------

QT       += core
QT+=network
QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    videoprocessor.cpp \
    server.cpp \
    processeddatasender.cpp \
    clientsession.cpp \
    cameramanager.cpp \
    camera.cpp
install_files.files+=config.json  hogcascade_pedestrians.xml combined.txt.model combined2.txt.model
install_files.path=$$OUT_PWD/
INSTALLS +=install_files

include(../common/common.pri)

HEADERS += \
    videoprocessor.h \
    server.h \
    processeddatasender.h \
    clientsession.h \
    cameramanager.h \
    camera.h

#unix|win32: LIBS += -L$$OUT_PWD/../PvdProcessor/ -lPvdProcessor
#INCLUDEPATH += $$PWD/../PvdProcessor
#DEPENDPATH += $$PWD/../PvdProcessor
#win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/PvdProcessor.lib
#else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/libPvdProcessor.a
# LIBS += -L$$OUT_PWD/../PvdProcessor/ -lPvdProcessor

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PvdProcessor/release/ -lPvdProcessor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PvdProcessor/debug/ -lPvdProcessor
else:unix: LIBS += -L$$OUT_PWD/../PvdProcessor/ -lPvdProcessor

INCLUDEPATH += $$PWD/../PvdProcessor
DEPENDPATH += $$PWD/../PvdProcessor

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/release/libPvdProcessor.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/debug/libPvdProcessor.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/release/PvdProcessor.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/debug/PvdProcessor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/libPvdProcessor.a
