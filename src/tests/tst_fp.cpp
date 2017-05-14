#include <catch.hpp>
#include <b0util/fp.hpp>

TEST_CASE("fp") {
    int a = 10;
    auto lambda = [&] (auto i, auto j) {
        return i * j + a;
    };
    auto vlambda = [&] () {};

    REQUIRE((b0::invoke(lambda, 2, 3) == 16));
    REQUIRE((b0::invoke_overflow(lambda, 2, 3, 4) == 16));

    using namespace b0::meta;

    META_ASSERT(is_invokable<decltype(lambda), int, int>::value);

    META_ASSERT(b0::meta::eq_v<void, b0::meta::invoke<decltype(vlambda)>::type>);
}
