#include <tst_future.hpp>

TEST_CASE("future_1") {
    SECTION("future_all") {
        SECTION("variadic") {
            b0::promise<void> promise;
            auto future = promise.future();
            auto f1 = future.then([] () {return 1;});
            auto f2 = future.then([] () {return std::string("text");});
            auto f3 = future.then([] () {return 0.5;});
            promise.resolve();
            REQUIRE(b0::future_all(f1, f2, f3).get() == std::make_tuple(1, std::string("text"), 0.5));
        }
        SECTION("list") {
            b0::promise<void> promise;
            auto future = promise.future();
            auto f1 = future.then([] () {return 1;});
            auto f2 = future.then([] () {return 2;});
            auto f3 = future.then([] () {return 3;});
            promise.resolve();
            REQUIRE((b0::future_all({f1, f2, f3}).get() == std::vector<int>{1, 2, 3}));
        }
    }
    SECTION("2.1.2.1: When fulfilled, a promise: must not transition to any other state.") {
        testFulfilled(s_magic, [] (auto future) {
            b0::future_state state = b0::future_state::unsettled;
            future.then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            }).wait();
            REQUIRE(state == b0::future_state::resolved);
        });
        SECTION("trying to fulfill then immediately reject") {
            b0::promise<void> promise;
            b0::future_state state = b0::future_state::unsettled;
            auto future = promise.future().then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            });
            promise.resolve();
            promise.reject();
            future.wait();
            REQUIRE(state == b0::future_state::resolved);
        }
        SECTION("trying to fulfill then reject, delayed") {
            b0::promise<void> promise;
            b0::future_state state = b0::future_state::unsettled;
            auto future = promise.future().then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            });
            async([=] () { promise.resolve(); promise.reject(); }, 100ms).wait();
            future.wait();
            REQUIRE(state == b0::future_state::resolved);
        }
        SECTION("trying to fulfill immediately then reject delayed") {
            b0::promise<void> promise;
            b0::future_state state = b0::future_state::unsettled;
            auto future = promise.future().then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            });
            promise.resolve();
            async([=] () { promise.reject(); }, 50ms).wait();
            future.wait();
            REQUIRE(state == b0::future_state::resolved);
        }
    }
    SECTION("2.1.3.1: When rejected, a promise: must not transition to any other state.") {
        testRejected<int>(0, [] (auto future) {
            b0::future_state state = b0::future_state::unsettled;
            future.then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            }).wait();
            REQUIRE(state == b0::future_state::rejected);
        });
        SECTION("trying to reject then immediately fulfill") {
            b0::promise<void> promise;
            b0::future_state state = b0::future_state::unsettled;
            auto future = promise.future().then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            });
            promise.reject();
            promise.resolve();
            future.wait();
            REQUIRE(state == b0::future_state::rejected);
        }
        SECTION("trying to reject then fulfill, delayed") {
            b0::promise<void> promise;
            b0::future_state state = b0::future_state::unsettled;
            auto future = promise.future().then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            });
            async([=] () {
                promise.reject();
                promise.resolve();
            }, 100ms).wait();
            future.wait();
            REQUIRE(state == b0::future_state::rejected);
        }
        SECTION("trying to reject immediately then fulfill delayed") {
            b0::promise<void> promise;
            b0::future_state state = b0::future_state::unsettled;
            auto future = promise.future().then([&] () {
                state = b0::future_state::resolved;
            }, [&] () {
                state = b0::future_state::rejected;
            });
            promise.reject();
            async([=] () {promise.resolve();}, 50ms).wait();
            future.wait();
            REQUIRE(state == b0::future_state::rejected);
       }
    }
    SECTION("2.2.1: Both `onFulfilled` and `onRejected` are optional arguments.") {
        // skip
        SECTION("2.2.1.1: If `onFulfilled` is not a function, it must be ignored.") {}
        SECTION("2.2.1.2: If `onRejected` is not a function, it must be ignored.") {}
    }
}
