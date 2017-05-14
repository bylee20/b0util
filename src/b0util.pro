TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = b0util

!CONFIG(no_test) {
    SUBDIRS += tests
}

QMAKE_CXXFLAGS += /wd4814
