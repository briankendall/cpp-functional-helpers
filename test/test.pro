QT += core
QT -= gui

CONFIG += c++11

TARGET = cpp-functional-helpers-test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
HEADERS += ../functionalHelpers.h \
           ../qtFunctionalHelpers.h
INCLUDEPATH += ..
