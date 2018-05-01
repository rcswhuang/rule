#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T22:21:59
#
#-------------------------------------------------
QT += widgets

TARGET = rule
TEMPLATE = app
#DESTDIR = ../lib
DEFINES += RULE_DLL

SOURCES += main.cpp\
        #mainwindow.cpp \
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
    hrulehandle.cpp \
    hrulewindow.cpp \
    hruledoc.cpp

HEADERS  += drawtool.h \
    #mainwindow.h \
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
    hrulehandle.h \
    hrulewindow.h \
    hruledoc.h

RESOURCES += \
    ruleeditor.qrc

FORMS += \
    logicprop.ui \
    bgprop.ui \
    viewprop.ui \
    simulateprop.ui \
    digitalprop.ui \
    compareprop.ui \
    rulewindow.ui
