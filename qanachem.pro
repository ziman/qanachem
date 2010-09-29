# -------------------------------------------------
# Project created by QtCreator 2010-09-28T18:22:25
# -------------------------------------------------
QT += opengl
TARGET = qanachem
TEMPLATE = app
LIBS += -lglut
SOURCES += main.cpp \
    mainwindow.cpp \
    glwidget.cpp \
    molecule.cpp
HEADERS += mainwindow.h \
    glwidget.h \
    molecule.h
FORMS += mainwindow.ui
RESOURCES += resources.qrc
