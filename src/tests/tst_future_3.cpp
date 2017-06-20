#include <tst_future.hpp>

TEST_CASE("promise_3") {
    {
        b0::future_unsafe<int> future;
        REQUIRE(!future);
        b0::promise_unsafe<int> promise;
        future = promise.future();
        REQUIRE(future);
        int value = 0;
        future.then([&value] (int v) { value = v; });
        promise.resolve(10);
        REQUIRE(value == 10);
    }

    SECTION("2.2.6: `then` may be called multiple times on the same promise.") {
        SECTION("2.2.6.1: If/when `promise` is fulfilled, "
                "all respective `onFulfilled` callbacks must execute "
                "in the order of their originating calls to `then`.") {
            SECTION("multiple boring fulfillment handlers") {
                testFulfilled(s_magic, [] (b0::future<int> future) {
                    int rejected = 0;
                    int results[] = {0, 0, 0};
                    auto f1 = future.then([&] (int v) { results[0] = v; }, [&] () { ++rejected; });
                    auto f2 = future.then([&] (int v) { results[1] = v; }, [&] () { ++rejected; });
                    auto f3 = future.then([&] (int v) { results[2] = v; }, [&] () { ++rejected; });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(results[0] == s_magic);
                    REQUIRE(results[1] == s_magic);
                    REQUIRE(results[2] == s_magic);
                    REQUIRE(rejected == 0);
                });
            }
            SECTION("multiple fulfillment handlers, one of which throws") {
                testFulfilled(s_magic, [] (b0::future<int> future) {
                    int rejected = 0;
                    int results[] = {0, 0, 0};
                    auto f1 = future.then([&] (int v) { results[0] = v; }, [&] () { ++rejected; });
                    auto f2 = future.then([&] (int v) { results[1] = v; throw -1; }, [&] () { ++rejected; });
                    auto f3 = future.then([&] (int v) { results[2] = v; }, [&] () { ++rejected; });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(results[0] == s_magic);
                    REQUIRE(results[1] == s_magic);
                    REQUIRE(results[2] == s_magic);
                    REQUIRE(rejected == 0);
                });
            }
            SECTION("results in multiple branching chains with their own fulfillment values") {
                testFulfilled(0, [] (b0::future<int> future) {
                    int results[] = {0, 0, 0};
                    auto f1 = future.then([&] () { return s_magic; }).then([&] (int v) { results[0] = v; });
                    auto f2 = future.then([&] () { throw s_magic; }).fail([&] (std::exception_ptr e) { results[1] = b0::unpack<int>(e); });
                    auto f3 = future.then([&] () { return s_magic; }).then([&] (int v) { results[2] = v; });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(results[0] == s_magic);
                    REQUIRE(results[1] == s_magic);
                    REQUIRE(results[2] == s_magic);
                });
            }
            SECTION("`onFulfilled` handlers are called in the original order") {
                testFulfilled(0, [] (b0::future<int> future) {
                    std::vector<int> order;
                    auto f1 = future.then([&] () { order.push_back(1); });
                    auto f2 = future.then([&] () { order.push_back(2); });
                    auto f3 = future.then([&] () { order.push_back(3); });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(order.size() == 3);
                    REQUIRE(order == (std::vector<int>{1, 2, 3}));
                });
                SECTION("even when one handler is added inside another handler") {
                    testFulfilled(0, [] (b0::future<int> future) {
                        std::vector<int> order;
                        b0::future<void> f3;
                        auto f1 = future.then([&] () {
                            order.push_back(1);
                            f3 = future.then([&] () { order.push_back(3); });
                        });
                        auto f2 = future.then([&] () { order.push_back(2); });
                        b0::future_all(f1, f2, f3).wait();
                        REQUIRE(order.size() == 3);
                        REQUIRE(order == (std::vector<int>{1, 3, 2}));
                    });
                }
            }
        }
        SECTION("2.2.6.2: If/when `promise` is rejected, "
                "all respective `onRejected` callbacks must execute "
                "in the order of their originating calls to `then`.") {
            SECTION("multiple boring rejection handlers") {
                testRejected<int>(s_magic, [] (b0::future<int> future) {
                    int results[] = {0, 0, 0};
                    int resolved = 0;
                    auto f1 = future.then([&] () { ++resolved; }, [&] (std::exception_ptr e) { results[0] = b0::unpack<int>(e); });
                    auto f2 = future.then([&] () { ++resolved; }, [&] (std::exception_ptr e) { results[1] = b0::unpack<int>(e); });
                    auto f3 = future.then([&] () { ++resolved; }, [&] (std::exception_ptr e) { results[2] = b0::unpack<int>(e); });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(results[0] == s_magic);
                    REQUIRE(results[1] == s_magic);
                    REQUIRE(results[2] == s_magic);
                    REQUIRE(resolved == 0);
                });
            }
            SECTION("multiple rejection handlers, one of which throws") {
                testRejected<int>(s_magic, [] (b0::future<int> future) {
                    int results[] = {0, 0, 0};
                    int resolved = 0;
                    auto f1 = future.then([&] () { ++resolved; }, [&] (std::exception_ptr e) { results[0] = b0::unpack<int>(e); });
                    auto f2 = future.then([&] () { ++resolved; }, [&] (std::exception_ptr e) { results[1] = b0::unpack<int>(e); std::rethrow_exception(e); });
                    auto f3 = future.then([&] () { ++resolved; }, [&] (std::exception_ptr e) { results[2] = b0::unpack<int>(e); });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(results[0] == s_magic);
                    REQUIRE(results[1] == s_magic);
                    REQUIRE(results[2] == s_magic);
                    REQUIRE(resolved == 0);
                });
            }
            SECTION("results in multiple branching chains with their own fulfillment values") {
                testRejected<int>(s_magic, [] (b0::future<int> future) {
                    int results[] = {0, 0, 0};
                    auto f1 = future.fail_or<int>([&] () { return 1; }).then([&] (int v) { results[0] = v; });
                    auto f2 = future.fail_or<int>([&] () { return 2; }).then([&] (int v) { results[1] = v; });
                    auto f3 = future.fail_or<int>([&] () { return 3; }).then([&] (int v) { results[2] = v; });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(results[0] == 1);
                    REQUIRE(results[1] == 2);
                    REQUIRE(results[2] == 3);
                });
            }

            SECTION("`onRejected` handlers are called in the original order") {
                testRejected<int>(s_magic, [] (b0::future<int> future) {
                    std::vector<int> order;
                    auto f1 = future.fail([&] () { order.push_back(1); });
                    auto f2 = future.fail([&] () { order.push_back(2); });
                    auto f3 = future.fail([&] () { order.push_back(3); });
                    b0::future_all(f1, f2, f3).wait();
                    REQUIRE(order.size() == 3);
                    REQUIRE(order == (std::vector<int>{1, 2, 3}));
                });
                SECTION("even when one handler is added inside another handler") {
                    testRejected<int>(s_magic, [] (b0::future<int> future) {
                        std::vector<int> order;
                        b0::future<void> f3;
                        auto f1 = future.fail([&] () {
                            order.push_back(1);
                            f3 = future.fail([&] () { order.push_back(3); });
                        });
                        auto f2 = future.fail([&] () { order.push_back(2); });
                        b0::future_all(f1, f2, f3).wait();
                        REQUIRE(order.size() == 3);
                        REQUIRE(order == (std::vector<int>{1, 3, 2}));
                    });
                }
            }
        }
    }
    SECTION("2.2.7: `then` must return a promise: `promise2 = promise1.then(onFulfilled, onRejected)`") {
        SECTION("is a promise") {
            auto f1 = b0::promise<void>().future();
            auto f2 = f1.then([] () {});
            REQUIRE(b0::is_future_v<decltype(f1)>);
            REQUIRE(b0::is_future_v<decltype(f2)>);
        }
        SECTION("2.2.7.1: If either `onFulfilled` or `onRejected` returns a value `x`, "
                "run the Promise Resolution Procedure `[[Resolve]](promise2, x)`") {
            SECTION("see separate 3.3 tests") {}
        }
        SECTION("2.2.7.2: If either `onFulfilled` or `onRejected` throws an exception `e`, "
                "`promise2` must be rejected with `e` as the reason.") {
            testReason(s_magic);
            testReason(std::string("test reason"));
        }
        SECTION("2.2.7.3: If `onFulfilled` is not a function and `promise1` is fulfilled, "
                "`promise2` must be fulfilled with the same value.") {}
        SECTION("2.2.7.4: If `onRejected` is not a function and `promise1` is rejected, "
                "`promise2` must be rejected with the same reason.") {}
    }
    SECTION("2.3.1: If `promise` and `x` refer to the same object, "
            "reject `promise` with a `TypeError' as the reason.") {
        SECTION("via return from a fulfilled promise") { /* ?? */ }
        SECTION("via return from a rejected promise") { /* ?? */ }
    }
    SECTION("2.3.2: If `x` is a promise, adopt its state") {
        SECTION("2.3.2.1: If `x` is pending, `promise` must remain pending until `x` is fulfilled or rejected.") {
            auto factory = [] () { return b0::promise<int>().future(); };
            testPromiseResolution(factory, [] (b0::future<int> future) {
                bool resolved = false, rejected = false;
                future.then([&] () { resolved = true; }, [&] () { rejected = true; });
                async([&] () {
                    REQUIRE(!resolved);
                    REQUIRE(!rejected);
                }, 100ms).wait();
            });
        }
        SECTION("2.3.2.2: If/when `x` is fulfilled, fulfill `promise` with the same value.") {
            SECTION("`x` is already-fulfilled") {
                auto factory = [] () { return resolved(s_magic); };
                testPromiseResolution(factory, [] (b0::future<int> future) {
                    int value = 0;
                    future.then([&] (int v) { value = v; }).wait();
                    REQUIRE(value == s_magic);
                });
            }
            SECTION("`x` is eventually-fulfilled") {
                auto factory = [] () { return _resolve(b0::promise<int>(), s_magic, 50ms); };
                testPromiseResolution(factory, [] (b0::future<int> future) {
                    int value = 0;
                    future.then([&] (int v) { value = v; }).wait();
                    REQUIRE(value == s_magic);
                });
            }
        }
        SECTION("2.3.2.3: If/when `x` is rejected, reject `promise` with the same reason.") {
            SECTION("`x` is already-rejected") {
                auto factory = [] () { return rejected<int>(s_magic); };
                testPromiseResolution(factory, [] (b0::future<int> future) {
                    int value = 0;
                    future.fail([&] (std::exception_ptr e) { value = b0::unpack<int>(e); }).wait();
                    REQUIRE(value == s_magic);
                });
            }
            SECTION("`x` is eventually-rejected") {
                auto factory = [] () { return _reject<int>(b0::promise<int>(), s_magic, 50ms); };
                testPromiseResolution(factory, [] (b0::future<int> future) {
                    int value = 0;
                    future.fail([&] (std::exception_ptr e) { value = b0::unpack<int>(e); }).wait();
                    REQUIRE(value == s_magic);
                });
            }
        }
    }

    SECTION("2.3.3: Otherwise, if `x` is an object or function,") {}
    SECTION("2.3.4: If `x` is not an object or function, fulfill `promise` with `x`") {}
}
