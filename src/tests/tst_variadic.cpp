#include <catch/catch.hpp>
#include <memory>
#include <functional>
#include <b0util/variadic.hpp>

TEST_CASE("variadic") {
    auto lt = [] (auto &&a, auto &&b) -> bool { return a < b; };
    REQUIRE(b0::max(1, 1.5, -1.1f) == 1.5);
    REQUIRE(b0::min(1, 1.5, -1.1f) == -1.1f);
    REQUIRE((b0::minmax(1, 1.5, -1.1f) == std::pair<double, double>(-1.1f, 1.5)));
    REQUIRE(b0::max_by(lt, 1, 1.5, -1.1f) == 1.5);
    REQUIRE(b0::min_by(lt, 1, 1.5, -1.1f) == -1.1f);
    REQUIRE((b0::minmax_by(lt, 1, 1.5, -1.1f) == std::pair<double, double>(-1.1f, 1.5)));

    REQUIRE(b0::all_of(1, 1, 1, 1));
    REQUIRE(!b0::all_of(1, 1, 2, 3));
    REQUIRE(b0::any_of(1, 0, 1, 2));
    REQUIRE(!b0::any_of(1, 0, -1, 2));
    REQUIRE(b0::none_of(1, 0, -1, 2));
    REQUIRE(!b0::none_of(1, 0, 1, 2));
}
