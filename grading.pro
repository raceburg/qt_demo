QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#opencv
INCLUDEPATH +=   /usr/local/include/opencv4
INCLUDEPATH +=   $$PWD
LIBS        +=   -L/usr/local/lib

LIBS += -lopencv_core \
        -lopencv_imgproc \
        -lopencv_imgcodecs \
        -lopencv_highgui

SOURCES += \
    src/contours_processing.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/polygonitem.cpp \
    src/statisticobjects.cpp \
    src/view.cpp \
    src/wascene.cpp \
    src/wascenegrid.cpp \
    src/wascenemanuallayer.cpp \
    src/wasceneruler.cpp

HEADERS += \
    src/contour_inf.h \
    src/contours_processing.h \
    src/gradingtask.h \
    src/mainwindow.h \
    src/messagetype.h \
    src/polygonitem.h \
    src/rulerstruct.h \
    src/statisticobjects.h \
    src/view.h \
    src/wascene.h \
    src/wascenegrid.h \
    src/wascenemanuallayer.h \
    src/wasceneruler.h \
    src/wavariables.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
