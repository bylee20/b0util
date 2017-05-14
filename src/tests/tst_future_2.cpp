#include <tst_future.hpp>

TEST_CASE("promise_2") {
    SECTION("2.2.2: If `onFulfilled` is a function,") {
        SECTION("2.2.2.1: it must be called after `promise` is fulfilled, "
                "with `promise`'s fulfillment value as its firtst argument.") {
            testFulfilled(s_magic, [] (b0::future<int> future) {
                int result = -1;
                future.then([&] (int value) {
                    result = value;
                }).wait();
                REQUIRE(result == s_magic);
            });
        }
        SECTION("2.2.2.2: it must not be called before `promise` is fulfilled") {
            SECTION("fulfilled after a delay") {
                b0::promise<int> promise;
                bool resolved = false;
                auto future = promise.future().then([&] () {
                    REQUIRE(!resolved);
                });
                async([&] () {
                    promise.resolve(s_magic);
                    resolved = true;
                }, 50ms).wait();
                future.wait();
                REQUIRE(resolved);
            }
            SECTION("never fulfilled") {
                b0::promise<int> promise;
                bool resolved = false;
                auto future = promise.future().then([&] () {
                    resolved = true;
                });
                std::this_thread::sleep_for(150ms);
                REQUIRE(!resolved);
            }
        }
        SECTION("2.2.2.3: it must not be called more than once.") {
            SECTION("already-fulfilled") {
                int called = 0;
                resolved(s_magic).then([&] () {
                    ++called;
                }).wait();
                REQUIRE(called == 1);
            }
            SECTION("trying to fulfill a pending promise more than once, immediately") {
                b0::promise<void> promise;
                int called = 0;
                auto future = promise.future().then([&] () {
                    ++called;
                });
                promise.resolve();
                promise.resolve();
                REQUIRE(called == 1);
            }
            SECTION("trying to fulfill a pending promise more than once, delayed") {
                b0::promise<void> promise;
                int called = 0;
                auto future = promise.future().then([&] () {
                    ++called;
                });
                async([=] () {
                    promise.resolve();
                    promise.resolve();
                }, 50ms).wait();
                future.wait();
                REQUIRE(called == 1);
            }
            SECTION("trying to fulfill a pending promise more than once, immediately then delayed") {
                b0::promise<void> promise;
                int called = 0;
                auto future = promise.future().then([&] () {
                    ++called;
                });
                promise.resolve();
                async([=] () {
                    promise.resolve();
                }, 50ms).wait();
                future.wait();
                REQUIRE(called == 1);
            }
            SECTION("when multiple `then` calls are made, spaced apart in time") {
                b0::promise<void> promise;
                int called[] = {0, 0, 0};
                auto future = promise.future();
                future.then([&] () {++called[0];});
                auto f1 = async([&] () {
                    future.then([&] () {++called[1];});
                }, 50ms);
                auto f2 = async([&] () {
                    future.then([&] () {++called[2];});
                }, 100ms);
                async([=] () {promise.resolve();}, 150ms).wait();
                f1.wait();
                f2.wait();
                future.wait();
                REQUIRE(called[0] == 1);
                REQUIRE(called[1] == 1);
                REQUIRE(called[2] == 1);
            }
            SECTION("when `then` is interleaved with fulfillment") {
                b0::promise<void> promise;
                int called[] = {0, 0};
                auto future = promise.future();
                auto f1 = future.then([&] () {++called[0];});
                promise.resolve();
                auto f2 = future.then([&] () {++called[1];});
                b0::future_all(f1, f2, future).wait();
                REQUIRE(called[0] == 1);
                REQUIRE(called[1] == 1);
            }
        }
    }
    SECTION("2.2.3: If `onRejected` is a function,") {
       SECTION("2.2.3.1: it must be called after `promise` is rejected, "
               "with `promise`'s rejection reason as its first argument.") {
           testRejected<int>(s_magic, [&] (b0::future<int> future) {
               int value = 0;
               future.fail([&] (std::exception_ptr e) { value = b0::unpack<int>(e); }).wait();
               REQUIRE(value == s_magic);
           });
       }
       SECTION("2.2.3.2: it must not be called before `promise` is rejected") {
           SECTION("rejected after a delay") {
               b0::promise<int> promise;
               bool rejected = false;
               auto future = promise.future().fail([&] () {
                   REQUIRE(!rejected);
               });
               async([&] () {
                   promise.reject();
                   rejected = true;
               }, 50ms).wait();
               future.wait();
               REQUIRE(rejected);
           }
           SECTION("never rejected") {
               b0::promise<int> promise;
               bool rejected = false;
               auto future = promise.future().fail([&] () {
                   rejected = true;
               });
               std::this_thread::sleep_for(150ms);
               REQUIRE(!rejected);
           }
       }
       SECTION("2.2.3.3: it must not be called more than once.") {
           SECTION("already-rejected") {
               int called = 0;
               rejected<void>("").fail([&] () {
                   ++called;
               }).wait();
               REQUIRE(called == 1);
           }
           SECTION("trying to reject a pending promise more than once, immediately") {
               b0::promise<void> promise;
               int called = 0;
               auto future = promise.future().fail([&] () {
                   ++called;
               });
               promise.reject();
               promise.reject();
               REQUIRE(called == 1);
           }
           SECTION("trying to reject a pending promise more than once, delayed") {
               b0::promise<void> promise;
               int called = 0;
               auto future = promise.future().fail([&] () {
                   ++called;
               });
               async([=] () {
                   promise.reject();
                   promise.reject();
               }, 50ms).wait();
               future.wait();
               REQUIRE(called == 1);
           }
           SECTION("trying to reject a pending promise more than once, immediately then delayed") {
               b0::promise<void> promise;
               int called = 0;
               auto future = promise.future().fail([&] () {
                   ++called;
               });
               promise.reject();
               async([=] () {
                   promise.reject();
               }, 50ms).wait();
               future.wait();
               REQUIRE(called == 1);
           }
           SECTION("when multiple `then` calls are made, spaced apart in time") {
               b0::promise<void> promise;
               int called[] = {0, 0, 0};
               auto future = promise.future();
               future.fail([&] () {++called[0];});
               auto f1 = async([&] () {
                   future.fail([&] () {++called[1];});
               }, 50ms);
               auto f2 = async([&] () {
                   future.fail([&] () {++called[2];});
               }, 100ms);
               async([=] () {promise.reject();}, 150ms).wait();
               f1.wait();
               f2.wait();
               future.wait();
               REQUIRE(called[0] == 1);
               REQUIRE(called[1] == 1);
               REQUIRE(called[2] == 1);
           }
           SECTION("when `then` is interleaved with rejection") {
               b0::promise<void> promise;
               int called[] = {0, 0};
               auto future = promise.future();
               auto f1 = future.fail([&] () {++called[0];});
               promise.reject();
               auto f2 = future.fail([&] () {++called[1];});
               b0::future_all(f1, f2, future).wait();
               REQUIRE(called[0] == 1);
               REQUIRE(called[1] == 1);
           }
       }
   }
   SECTION("2.2.4: `onFulfilled` or `onRejected` must not be called "
           "until the execution context stack contains only platform code.") {}
   SECTION("2.2.5 `onFulfilled` and `onRejected` must be called as functions (i.e. with no `this` value).") {}
}
