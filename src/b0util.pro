TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = b0util

!CONFIG(no_test) {
    SUBDIRS += tests
}
