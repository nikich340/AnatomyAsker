TEMPLATE     = app
HEADERS	     = AnatomyAsker.h GraphicsView.h
SOURCES	     = AnatomyAsker.cpp GraphicsView.cpp main.cpp
QT          += widgets xml
RESOURCES   += res1.qrc res2.qrc res3.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
windows:TARGET = ../AnatomyAsker

