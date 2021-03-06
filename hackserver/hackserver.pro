#-------------------------------------------------
#
# Project created by QtCreator 2018-04-14T21:03:38
#
#-------------------------------------------------

QT += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hackserver
TEMPLATE = app
LIBS += -L/usr/local/lib -lprotobuf
CONFIG += c++11


SOURCES += main.cpp \
        hackserver.cpp \
        ../proto/hackmessage.pb.cc \
    objectdao.cpp

HEADERS  += hackserver.h \
          ../proto/hackmessage.pb.h \
    objectdao.h

FORMS    += hackserver.ui
