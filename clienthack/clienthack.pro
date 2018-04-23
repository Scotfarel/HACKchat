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

CONFIG += c++11

SOURCES += main.cpp\
        client.cpp

HEADERS  += client.h

FORMS    += client.ui
