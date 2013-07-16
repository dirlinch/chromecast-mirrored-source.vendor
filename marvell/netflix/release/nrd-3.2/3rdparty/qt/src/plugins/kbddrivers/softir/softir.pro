TARGET = qsoftirkbddriver
include(../../qpluginbase.pri)

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/kbddrivers
target.path = $$[QT_INSTALL_PLUGINS]/kbddrivers
INSTALLS += target

DEFINES += QT_QWS_KBD_SOFT_IR  

HEADERS	= qkbdsoftir.h

SOURCES	= main.cpp qkbdsoftir.cpp NRD_IRReceiver.cpp

QT += xml
