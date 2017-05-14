#include <catch.hpp>
#include <iostream>
#include <variant.hpp>

using namespace b0;

TEST_CASE("variant") {
    using var_t = b0::variant<int, std::string, double>;
    var_t var("asd");

    REQUIRE(var.index() == 1);
    REQUIRE(var.get<std::string>() == "asd");

    var = "text";
    REQUIRE(var.get<std::string>() == "text");

    var = 0.5;
    REQUIRE(var.is_holding<double>());
    REQUIRE(!var.is_holding<int>());
    REQUIRE(!var.is_holding<std::string>());
    REQUIRE(var.get<double>() == 0.5);
    REQUIRE(var.get_or(0.1) == 0.5);
    REQUIRE(var.type_id() == b0::type_id<double>());
}
