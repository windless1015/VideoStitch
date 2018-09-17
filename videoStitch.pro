#-------------------------------------------------
#
# Project created by QtCreator 2018-09-17T14:08:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = videoStitch
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    CaptureThread.cpp \
    ImageBuffer.cpp

HEADERS  += mainwindow.h \
    CaptureThread.h \
    ImageBuffer.h

FORMS    += mainwindow.ui

INCLUDEPATH+= F:/all_libs/opencv/build/include \
                   F:/all_libs/opencv/build/include/opencv \
                   F:/all_libs/opencv/build/include/opencv2


LIBS+=F:\all_libs\opencv\build\x86\vc12\lib\opencv_core2413d.lib
LIBS+=F:\all_libs\opencv\build\x86\vc12\lib\opencv_highgui2413d.lib
LIBS+=F:\all_libs\opencv\build\x86\vc12\lib\opencv_imgproc2413d.lib
