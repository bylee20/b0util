#include <catch.hpp>
#include <tuple>
#include "any.hpp"

TEST_CASE("Any") {
    using namespace b0::meta;

    b0::any any;
    REQUIRE(!any.has_value());

    any = b0::make_any(1);
    REQUIRE(any.value<int>() == 1);

    any = std::string("test");
    REQUIRE(any.value<std::string>() == "test");
    REQUIRE(any.value_or<std::string>("asd") == "test");
    REQUIRE(any.value_or(10) == 10);

    using type = std::tuple<int, double>;
    any.emplace<type>(13, 0.5);
    REQUIRE(std::get<0>(any.value<type>()) == 13);
    REQUIRE(std::get<1>(any.value<type>()) == 0.5);

    auto other = any;
    any.reset();
    REQUIRE(!any.has_value());
    REQUIRE(std::get<1>(other.value<type>()) == 0.5);
    REQUIRE(std::get<0>(other.value<type>()) == 13);
}
