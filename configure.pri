CONFIG += conan_basic_setup
include(conanbuildinfo.pri)

BIN_DIR = $$PWD/build/bin
LIB_DIR = $$PWD/build/lib

INCLUDEPATH += $$PWD
LIBS        += -L$$LIB_DIR

CONFIG(debug, debug|release) {
    DEBUG_BUILD = true
}

equals(TEMPLATE, lib) {
    DESTDIR     = $$LIB_DIR
    DLLDESTDIR  = $$BIN_DIR
} else:equals(TEMPLATE, app) {
    DESTDIR     = $$BIN_DIR
}

QMAKE_CXXFLAGS += /wd4814
