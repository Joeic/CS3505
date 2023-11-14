QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    animation.cpp \
    animationpreview.cpp \
    eraser.cpp \
    frame.cpp \
    frameitemdelegate.cpp \
    main.cpp \
    mainwindow.cpp \
    model.cpp \
    paint.cpp \
    paintbrush.cpp \
    paintbucket.cpp \
    tool.cpp \
    undostate.cpp

HEADERS += \
    animation.h \
    animationpreview.h \
    eraser.h \
    frame.h \
    frameitemdelegate.h \
    mainwindow.h \
    model.h \
    paint.h \
    paintbrush.h \
    paintbucket.h \
    tool.h \
    undostate.h

FORMS += \
    animationpreview.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
