TARGET = tst_namespace_custom
SOURCES += $$PWD/tst_custom.cpp
DEFINES += QTPROMISE_NAMESPACE=Foo
DEFINES -= QTPROMISE_NO_NAMESPACE

include(../../qtpromise.pri)
