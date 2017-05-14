TEMPLATE = app
include(../common.pri)

TARGET = tests

QT -= gui
CONFIG += c++14
LIBS += -lb0util

INCLUDEPATH += ../src

SOURCES += \
    tst_rho.cpp \
    tst_symbol.cpp \
    tst_jsonio.cpp \
    tst_algorithm.cpp \
    main.cpp \
    tst_meta.cpp \
    tst_optional.cpp \
    tst_any.cpp \
    tst_variant.cpp \
    tst_fp.cpp \
    tst_future_1.cpp \
    tst_future_2.cpp \
    tst_future_3.cpp \
    tst_concurrent.cpp \
    tst_range.cpp \
    tst_unicode.cpp

HEADERS += \
    tst_future.hpp


QMAKE_CXXFLAGS += /wd4814
