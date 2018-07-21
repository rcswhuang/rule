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
INCLUDEPATH += \
            ../include

SOURCES +=  \
    #main.cpp\
        #mainwindow.cpp \
    hlogicprop.cpp \
    #hbgprop.cpp \
    hviewprop.cpp \
    #hsimulateprop.cpp \
    hdigitalprop.cpp \
    hcompareprop.cpp \
    hrulehandle.cpp \
    hrulewindow.cpp \
    hruledoc.cpp \
    hlookrulereport.cpp \
    hexportrule.cpp \
    hdrawobj.cpp \
    hdrawtool.cpp \
    hframe.cpp \
    hrulefile.cpp

HEADERS  += \
    #mainwindow.h \
    hlogicprop.h \
    #hbgprop.h \
    hviewprop.h \
    #hsimulateprop.h \
    hdigitalprop.h \
    hcompareprop.h \
    hrulehandle.h \
    hrulewindow.h \
    hruledoc.h \
    hlookrulereport.h \
    hexportrule.h \
    hdrawobj.h \
    hdrawtool.h \
    hframe.h \
    hrulefile.h
    #hruleeditapi.h \
    #hruleexport.h

RESOURCES += \
    ruleeditor.qrc

FORMS += \
    logicprop.ui \
    #bgprop.ui \
    viewprop.ui \
    #simulateprop.ui \
    digitalprop.ui \
    compareprop.ui \
    rulewindow.ui \
    lookrulereport.ui \
    exportrule.ui
