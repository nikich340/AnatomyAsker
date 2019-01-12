TEMPLATE     = app
HEADERS	     = AnatomyAsker.h GraphicsView.h
SOURCES	     = AnatomyAsker.cpp GraphicsView.cpp main.cpp
QT          += widgets xml multimedia
RESOURCES   += commonRes.qrc \
               osteoRes1.qrc osteoRes2.qrc osteoRes3.qrc osteoRes4.qrc osteoRes5.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
windows:TARGET = ../AnatomyAsker

