TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = src

!CONFIG(no_test) {
    SUBDIRS += tests
}

QMAKE_CXXFLAGS += /wd4814
