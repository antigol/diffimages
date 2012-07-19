#-------------------------------------------------
#
# Project created by QtCreator 2012-07-04T11:42:42
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = diffimages
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    histogram.cpp \
    openimagesdialog.cpp \
    glwidget.cpp \
    imageviewer.cpp \
    multihistogram.cpp

HEADERS  += \
    mainwindow.h \
    histogram.h \
    openimagesdialog.h \
    glwidget.h \
    imageviewer.h \
    multihistogram.h

RESOURCES += \
    files.qrc

OTHER_FILES += \
    sgfilter.frag \
    subtraction.frag \
    flat.vert \
    render.frag \
    render3D.vert \
    render3D.frag \
    viewer.frag

FORMS += \
    mainwindow.ui \
    openimagesdialog.ui
