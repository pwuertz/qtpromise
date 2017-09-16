TARGET = qtpromiseplugin
TARGETPATH = QtPromise
IMPORT_VERSION = 1.0
QT += qml

DEFINES += QTQMLPROMISE_LIBRARY

include(../qtqmlpromise/qtqmlpromise.pri)
include(../../qtpromise.pri)

SOURCES += $$PWD/plugin.cpp

qmlpromise.files = $$PWD/qtqmlpromise.js
qmlpromise.prefix = /QtPromise
RESOURCES += qmlpromise

CONFIG += no_cxx_module
load(qml_plugin)

CONFIG += exceptions
