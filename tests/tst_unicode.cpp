#include <catch.hpp>
#include <unicode.hpp>

TEST_CASE("unicode") {
    REQUIRE(b0::to_utf8(L"abc", 3) == "abc");
    REQUIRE(b0::from_utf8("abc", 3) == L"abc");
}
