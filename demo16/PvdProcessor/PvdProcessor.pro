#-------------------------------------------------
#
# Project created by QtCreator 2018-04-09T11:23:30
#
#-------------------------------------------------


QT       -= core gui
CONFIG+=c++11
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


win32{
OPENCV_PATH=E:\repo-github\libs\third-party\opencv\opencv-vs2013-x86
    INCLUDEPATH +="$$OPENCV_PATH\include"
# LIBS+=-L$$OPENCV_PATH\lib   -lopencv_core249 -lopencv_highgui249 \
#        -lopencv_objdetect249 -lopencv_imgproc249 -lopencv_ml249  -lopencv_video249
 LIBS+=-L$$OPENCV_PATH\lib   -lopencv_core249d -lopencv_highgui249d \
        -lopencv_objdetect249d -lopencv_imgproc249d -lopencv_ml249d  -lopencv_video249d
INCLUDEPATH+="C:\Program Files\Microsoft SDKs\Windows\v7.1A\Include"
}
