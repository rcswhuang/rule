#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T22:21:59
#
#-------------------------------------------------
QT += widgets

TARGET = rule
TEMPLATE = lib
DESTDIR = ../lib
DEFINES += RULE_DLL

SOURCES += \ #main.cpp\
        mainwindow.cpp \
    drawtool.cpp \
    drawobj.cpp \
    rulefile.cpp \
    frame.cpp \
    hlogicprop.cpp \
    hbgprop.cpp \
    hviewprop.cpp \
    hsimulateprop.cpp \
    hdigitalprop.cpp \
    hcompareprop.cpp \
    hrulehandle.cpp

HEADERS  += mainwindow.h \
    drawtool.h \
    drawobj.h \
    rulefile.h \
    ruleexport.h \
    frame.h \
    ruleeditapi.h \
    hlogicprop.h \
    hbgprop.h \
    hviewprop.h \
    hsimulateprop.h \
    hdigitalprop.h \
    hcompareprop.h \
    hrulehandle.h

RESOURCES += \
    ruleeditor.qrc

FORMS += \
    logicprop.ui \
    bgprop.ui \
    viewprop.ui \
    simulateprop.ui \
    digitalprop.ui \
    compareprop.ui
