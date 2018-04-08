#-------------------------------------------------
#
# Project created by QtCreator 2018-03-20T22:20:24
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    videoprocessor.cpp \
    server.cpp \
    processeddatasender.cpp \
    packagemanager.cpp \
    fvdvideoprocessor.cpp \
    clientsession.cpp \
    cameramanager.cpp \
    camera.cpp \
    pddemoprocessor.cpp \
    testpd.cpp \
    pdprocessor.cpp

install_files.files+=config.json  hogcascade_pedestrians.xml combined.txt.model combined2.txt.model
install_files.path=$$OUT_PWD/
INSTALLS +=install_files

include(../common/common.pri)

HEADERS += \
    videoprocessor.h \
    server.h \
    processeddatasender.h \
    packagemanager.h \
    fvdvideoprocessor.h \
    clientsession.h \
    cameramanager.h \
    camera.h \
    pddemoprocessor.h \
    testpd.h \
    pdprocessor.h
