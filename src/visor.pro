#-------------------------------------------------
#
# Project created by QtCreator 2012-11-11T17:01:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visor
TEMPLATE = app
#CONFIG += qt warn_on qt3d
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /usr/include/eigen3

LIBS += -lOpenMeshCore -lOpenMeshTools -lGL -lGLU -lglut -lQtOpenGL

SOURCES += main.cpp\
        mainwindow.cpp \
    QGLViewerWidget.cc \
    MeshViewerWidgetT.cc \
    colormap.cpp \
    axis3d.cpp \
    PrincipalAxisGenerator.cpp \
    LinalgTools.cpp \
    qprincipalaxisgenerator.cpp \
    MeshTranslator.cpp

HEADERS  += mainwindow.h \
    MeshViewerWidgetT.hh \
    MeshViewerWidget.hh \
    QGLViewerWidget.hh \
    qdebugstream.h \
    colormap.h \
    axis3d.h \
    PrincipalAxisGenerator.h \
    LinalgTools.h \
    mesh.h \
    qprincipalaxisgenerator.h \
    MeshTranslator.h

FORMS    += mainwindow.ui
