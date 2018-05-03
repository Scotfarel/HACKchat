#-------------------------------------------------
#
# Project created by QtCreator 2018-04-11T22:02:26
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clienthack
TEMPLATE = app
LIBS += -L/usr/local/lib -lprotobuf
CONFIG += c++11

SOURCES += main.cpp \
        client.cpp \
        ../proto/hackmessage.pb.cc

HEADERS  += client.h \
        ../proto/hackmessage.pb.h

FORMS    += client.ui
