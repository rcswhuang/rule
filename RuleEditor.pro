#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T22:21:59
#
#-------------------------------------------------
QT += widgets

TARGET = RuleEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    drawtool.cpp \
    drawobj.cpp \
    rulefile.cpp \
    frame.cpp \
    inputprop.cpp

HEADERS  += mainwindow.h \
    drawtool.h \
    drawobj.h \
    rulefile.h \
    ruleexport.h \
    frame.h \
    inputprop.h \
    ruleeditapi.h

RESOURCES += \
    ruleeditor.qrc

FORMS += \
    inputprop.ui
