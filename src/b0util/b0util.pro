TEMPLATE = lib
include(../common.pri)

TARGET = b0util

QT -= gui
CONFIG += c++14
DEFINES += B0UTIL_LIBRARY

SOURCES += \
    thread_pool.cpp \
    log.cpp \
    unicode.cpp \
    path.cpp

HEADERS +=\
    fp.hpp \
    jsonio.hpp \
    op.hpp \
    private/jsonio_p.hpp \
    timing.hpp \
    meta/meta.hpp \
    meta/type_seq.hpp \
    meta/int_seq.hpp \
    optional.hpp \
    any.hpp \
    type_id.hpp \
    utility.hpp \
    variant.hpp \
    future.hpp \
    meta/_invoke.hpp \
    private/_future_data.hpp \
    private/_future_helper.hpp \
    assert.hpp \
    symbol/symbol.hpp \
    symbol/object.hpp \
    symbol/factory.hpp \
    meta/container.hpp \
    meta/callable.hpp \
    private/_future_state.hpp \
    thread_pool.hpp \
    range/map.hpp \
    container.hpp \
    range/filter.hpp \
    range/_range_fn.hpp \
    range/counter.hpp \
    range/_terminal_fn.hpp \
    range/reduce.hpp \
    range/count.hpp \
    range/view.hpp \
    range/limit.hpp \
    range/to.hpp \
    meta/iterator.hpp \
    range/pipe.hpp \
    range/iterable.hpp \
    range/copy.hpp \
    async.hpp \
    range/find.hpp \
    range/for_each.hpp \
    log.hpp \
    unicode.hpp \
    global.hpp \
    path.hpp \
    range/flatten.hpp \
    variadic.hpp \
    range.hpp
