CONFIG += conan_basic_setup
include(../conanbuildinfo.pri)

CONFIG(debug, debug|release) {
    DEBUG_BUILD = true
    BIN_DIR = $$PWD/../build/debug/bin
    LIB_DIR = $$PWD/../build/debug/lib
} else {
    BIN_DIR = $$PWD/../build/release/bin
    LIB_DIR = $$PWD/../build/release/lib
}

INCLUDEPATH += $$PWD
LIBS        += -L$$LIB_DIR

equals(TEMPLATE, lib) {
    DESTDIR     = $$LIB_DIR
    DLLDESTDIR  = $$BIN_DIR
} else:equals(TEMPLATE, app) {
    DESTDIR     = $$BIN_DIR
}

equals(QMAKE_CXX, clang-cl) {
QMAKE_CXXFLAGS += -Xclang -std=c++1z -Xclang -Wno-expansion-to-defined
}

QMAKE_CXXFLAGS += /wd4814
