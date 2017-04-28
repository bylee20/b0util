#pragma once

#include <catch.hpp>
#include <future.hpp>
#include <future>
#include <chrono>

using namespace std::chrono;
using namespace std::chrono_literals;

static int s_magic = 0x12345678;

template<class F>
static auto async(F &&func, milliseconds msec = 0ms) -> auto
{
    return std::async(std::launch::async, [=, f=std::forward<F>(func)] () {
        if (msec.count() > 0)
            std::this_thread::sleep_for(msec);
        f();
    });
}

template<class T>
static auto resolved(const T &t) -> b0::future<T>
{
    b0::promise<T> promise;
    promise.resolve(t);
    return promise.future();
}

template<class T, class E>
static auto rejected(const E &e) -> b0::future<b0::meta::wrap_void_t<T>>
{
    b0::promise<T> promise;
    promise.reject(e);
    return promise.future();
}

template<class T>
static auto _resolve(b0::promise<T> promise, const T &value,
                    milliseconds msec = 0ms) -> b0::future<T>
{ async([=] () { promise.resolve(value); }, msec); return promise.future(); }

template<class T, class E>
static auto _reject(b0::promise<T> promise, const E &e,
                   milliseconds msec = 0ms) -> b0::future<T>
{ async([=] () { promise.reject(e); }, msec); return promise.future(); }


template<class T, class F>
auto testFulfilled(const T &t, F &&func) -> void
{
    SECTION("already-fulfilled") { func(b0::make_resolve(t)); }
    SECTION("immediately-fulfilled") { func(_resolve(b0::promise<T>(), t)); }
    SECTION("eventually-fulfilled") { func(_resolve(b0::promise<T>(), t, 50ms)); }
}

template<class T, class E, class F>
auto testRejected(const E &e, F &&func) -> void
{
    SECTION("already-rejected") { func(b0::make_reject<T>(e)); }
    SECTION("immediately-rejected") { func(_reject(b0::promise<T>(), e)); }
    SECTION("eventually-rejected") { func(_reject(b0::promise<T>(), e, 50ms)); }
};

template<class T>
static auto to_string(T &&t) -> std::string { return std::to_string(std::forward<T>(t)); }
static auto to_string(const std::string &s) -> std::string { return s; }

template<class E>
static auto testReason(const E &e) {
    SECTION("The reason is " + to_string(e)) {
        testFulfilled(0, [=] (b0::future<int> future) {
            auto f = future.then([=] () { throw e; });
            E error{};
            f.fail([&] (std::exception_ptr e) { error = b0::unpack<E>(e); }).wait();
            REQUIRE(error == e);
        });
        testRejected<int>(0, [=] (b0::future<int> future) {
            auto f = future.fail([=] () { throw e; });
            E error{};
            f.fail([&] (std::exception_ptr e) { error = b0::unpack<E>(e); }).wait();
            REQUIRE(error == e);
        });
    }
}

template<class Factory, class Test>
static auto testPromiseResolution(Factory &&factory, Test &&test) {
    SECTION("via return from a fulfilled promise") {
        auto future = resolved(0).then([=] () { return factory(); });
        test(future);
    }
    SECTION("via return from a rejected promise") {
        using T = typename std::decay_t<decltype(factory())>::value_type;
        auto future = rejected<int>(0).template fail_or<T>([=] () { return factory(); });
        test(future);
    }
}

