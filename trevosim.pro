#-------------------------------------------------
#
# Project created by QtCreator 2016-07-19T11:47:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TREvoSim
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    organism.cpp \
    randoms.cpp \
    settings.cpp \
    simulation_variables.cpp \
    output.cpp \
    about.cpp \
    darkstyletheme.cpp

HEADERS  += mainwindow.h \
    organism.h \
    randoms.h \
    settings.h \
    simulation_variables.h \
    output.h \
    about.h \
    version.h \
    darkstyletheme.h

FORMS    += mainwindow.ui \
    settings.ui \
    output.ui \
    about.ui

RESOURCES += \
    resources.qrc

#Windows icon for executable
RC_FILE = resources/trevosim.rc

#Needed to use C++ sort algorithm with lamda function
CONFIG += c++11

#Mac icon
ICON = ./resources/icon.icns

#Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
QMAKE_LFLAGS += -no-pie
