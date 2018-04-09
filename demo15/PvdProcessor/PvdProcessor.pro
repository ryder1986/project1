#-------------------------------------------------
#
# Project created by QtCreator 2018-04-09T11:23:30
#
#-------------------------------------------------
CONFIG +=c++11
QT       -= core gui

TARGET = PvdProcessor
TEMPLATE = lib
CONFIG += staticlib

SOURCES += pvdprocessor.cpp \
    teststaticlib.cpp C4Common.cpp

HEADERS += pvdprocessor.h \
    teststaticlib.h  C4Common.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
unix{
DEFINES +=IS_UNIX
QMAKE_CXXFLAGS+=-w
}
unix{
CVPATH="/root/repo-github/libs/third-party/opencv"
CV_PREFIX="/opencv-linux-x86"
ALGPATH="/root/repo-github/libs/alg"
}

unix{
    DEFINES += IS_UNIX
    QMAKE_CXXFLAGS+="-std=c++11"
    INCLUDEPATH +="$$CVPATH/$$CV_PREFIX/include"
    INCLUDEPATH +="$$ALGPATH/include"
    message(alg  in $$ALGPATH/include)
    LIBS+=-L$$CVPATH/$$CV_PREFIX/lib -lopencv_core -lopencv_highgui \
       -lopencv_objdetect -lopencv_imgproc -lopencv_ml   -lopencv_video  -lX11

    QMAKE_LIBS+=-lpthread
}
   INCLUDEPATH +=../server ../common
