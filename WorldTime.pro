#-------------------------------------------------
#
# Project created by QtCreator 2023-10-20T13:54:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorldTime
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp \
           worldtimewidget.cpp

HEADERS  += mainwindow.h \
            worldtimewidget.h

FORMS    += mainwindow.ui \
            worldtimewidget.ui \
            infowidget.ui

RESOURCES += \
    rc.qrc
