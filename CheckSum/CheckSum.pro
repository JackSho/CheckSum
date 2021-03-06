######################################################################
# Automatically generated by qmake (3.0) ?? 9? 23 10:44:01 2016
######################################################################

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CheckSum
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# Input

SOURCES += CalculateFileCheckThread.cpp \
           CalculateTaskManager.cpp \
           checksum.cpp \
           main.cpp \
           MoveLabel.cpp \
           MoveWidget.cpp \
           QtWindowsAero.cpp \
           singleapplication.cpp \
           qrc_checksum.cpp

HEADERS += CalculateFileCheckThread.h \
           CalculateTaskManager.h \
           checksum.h \
           MoveLabel.h \
           MoveWidget.h \
           QtWindowsAero.h \
           resource.h \
           singleapplication.h \
           ui_checksum.h

RESOURCES += checksum.qrc

FORMS += checksum.ui
