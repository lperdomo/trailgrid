#-------------------------------------------------
#
# Project created by QtCreator 2017-05-02T09:52:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = firstoccupgrd
TEMPLATE = app


SOURCES += main.cpp\
    bot.cpp \
    occupancygrid.cpp \
    controller.cpp \
    scenegrid.cpp \
    keyboard.cpp \
    pid.cpp

HEADERS  += \
    bot.h \
    occupancygrid.h \
    controller.h \
    scenegrid.h \
    keyboard.h \
    pid.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/Aria/lib/release/ -lAria
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/Aria/lib/debug/ -lAria
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/Aria/lib/ -lAria

INCLUDEPATH += $$PWD/../../../../../../usr/local/Aria/include
DEPENDPATH += $$PWD/../../../../../../usr/local/Aria/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/Aria/lib/release/ -lArNetworking
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/local/Aria/lib/debug/ -lArNetworking
else:unix: LIBS += -L$$PWD/../../../../../../usr/local/Aria/lib/ -lArNetworking

INCLUDEPATH += $$PWD/../../../../../../usr/local/Aria/ArNetworking/include
DEPENDPATH += $$PWD/../../../../../../usr/local/Aria/ArNetworking/include
