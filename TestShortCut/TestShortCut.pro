#-------------------------------------------------
#
# Project created by QtCreator 2019-08-26T17:12:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestShortCut
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
    linepaint.cpp \
        mainwindow.cpp \
    keystring.cpp \
    canvas.cpp \
    operateset.cpp \
    rectpaint.cpp \
    textpaint.cpp

HEADERS  += mainwindow.h \
    keystring.h \
    canvas.h \
    linepaint.h \
    operateset.h \
    rectpaint.h \
    textpaint.h

FORMS    += mainwindow.ui

include(QHotkey/qhotkey.pri)

SUBDIRS += \
    QHotkey/QHotkey.pro \
    QHotkey/QHotkey.pro

RESOURCES += \
    resouce.qrc












