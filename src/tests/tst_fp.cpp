#include <catch/catch.hpp>
#include <memory>
#include <functional>
#include <b0util/fp.hpp>

TEST_CASE("fp") {
    struct A { auto memfun(int a) const -> int { return a + 1; } };

    A o;

    REQUIRE(b0::invoke(&A::memfun, o, 0) == 1);
    REQUIRE(b0::invoke(&A::memfun, &o, 0) == 1);
    REQUIRE(b0::invoke(&A::memfun, std::cref(o), 0) == 1);
    REQUIRE(b0::invoke(&A::memfun, std::ref(o), 0) == 1);
    REQUIRE(b0::invoke(&A::memfun, std::make_shared<A>(), 0) == 1);
    REQUIRE(b0::invoke(std::mem_fn(&A::memfun), o, 0) == 1);

    REQUIRE(b0::invoke_overflow(&A::memfun, o, 0, 1) == 1);
    REQUIRE((b0::meta::is_invokable_overflow<decltype(&A::memfun), A, int>::value));
    REQUIRE((b0::meta::is_invokable_overflow<decltype(&A::memfun), A, int, std::string>::value));

    META_ASSERT((b0::meta::eq_v<b0::meta::invoke_overflow_t<decltype(&A::memfun), A, int>, int>));
    META_ASSERT((b0::meta::eq_v<b0::meta::invoke_overflow_t<decltype(&A::memfun), A, int, std::string>, int>));

    REQUIRE(!(b0::meta::is_invokable_overflow<decltype(&A::memfun), A>::value));
    REQUIRE(!(b0::meta::is_invokable_overflow<decltype(&A::memfun), A, std::string>::value));
    REQUIRE(!(b0::meta::is_invokable_overflow<void(*)(int), std::string>::value));

    int a = 10;
    auto lambda = [&] (auto i, auto j) {
        return i * j + a;
    };
    auto vlambda = [&] () {};

    REQUIRE((b0::invoke(lambda, 2, 3) == 16));
//    b0::meta::detail::invoke_overflow_impl(lambda, 2, 3, 4);
//    REQUIRE((b0::invoke_overflow(lambda, 2, 3, 4) == 16));

    using namespace b0::meta;

    META_ASSERT(is_invokable<decltype(lambda), int, int>::value);
//    META_ASSERT(!is_invokable_overflow_v<decltype(lambda), int>);

    META_ASSERT(b0::meta::eq_v<void, b0::meta::invoke<decltype(vlambda)>::type>);


    auto tuple = std::make_tuple(1, 2, 3.0);
    int val = 0;
    b0::for_each(tuple, [&] (auto v) { val += v; });
    REQUIRE(val == 6);
}
