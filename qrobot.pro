#-------------------------------------------------
#
# Project created by QtCreator 2019-03-15T10:53:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qrobot
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
    comsetupdlg.cpp \
    mainview.cpp \
    console.cpp \
    geometryengine.cpp \
    uartctrl.cpp \
    servo.cpp \
    inifile/inifile.cpp \
    controlpage.cpp \
    pagecali.cpp \
    pageservo.cpp \
    helpdlg.cpp \
    pageseq.cpp \
    pageplay.cpp

HEADERS  += mainwindow.h \
    comsetupdlg.h \
    mainview.h \
    console.h \
    geometryengine.h \
    uartctrl.h \
    servo.h \
    common.h \
    inifile/inifile.h \
    controlpage.h \
    pagecali.h \
    helpdlg.h \
    pageservo.h \
    pageseq.h \
    pageplay.h

FORMS    += mainwindow.ui \
    comsetupdlg.ui \
    calibrate.ui \
    servoctrl.ui \
    helpdlg.ui \
    pageseq.ui \
    pageplay.ui

RESOURCES += \
    images.qrc \
    textures.qrc \
    shaders.qrc

DISTFILES +=
