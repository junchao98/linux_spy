#-------------------------------------------------
#
# Project created by QtCreator 2018-04-13T16:13:45
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += xml


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = socket
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    qt_aes.cpp \
    msg_core/list.c \
    msg_core/msg_core.c \
    xml.cpp \
    baidu_map.cpp \
    send_file.cpp

HEADERS  += mainwindow.h \
    qt_aes.h \
    msg_core/common.h \
    msg_core/list.h \
    msg_core/msg_core.h \
    msg_core/sensor_com.h \
    xml.h \
    baidu_map.h \
    send_file.h

FORMS    += mainwindow.ui \
    send_file.ui

RESOURCES += \
    res.qrc

DISTFILES +=
