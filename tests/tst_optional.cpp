#include <catch.hpp>
#include "optional.hpp"


TEST_CASE("Optional") {
    {
        auto opt = b0::make_optional(1);
        META_ASSERT(std::is_base_of_v<b0::detail::optional_storage_value<int>, decltype(opt)>);
        META_ASSERT(sizeof(opt) == 8);
        REQUIRE(opt);
        REQUIRE(*opt == 1);
        opt = 10;
        REQUIRE(*opt == 10);
        auto opt2 = opt;
        REQUIRE(opt == opt2);
        opt = b0::nullopt;
        REQUIRE(!opt);
        REQUIRE(opt.value_or(-1) == -1);
        REQUIRE(opt2.value_or(-1) == 10);
        REQUIRE(*opt2 == 10);
        opt2 = {};
        REQUIRE(!opt2);
        REQUIRE(opt == opt2);
        opt2 = 11;
        opt2.swap(opt);
        REQUIRE(*opt == 11);
    }
    {
        auto opt = b0::make_optional<std::string>("text");
        META_ASSERT(std::is_base_of_v<b0::detail::optional_storage_pointer<std::string>, decltype(opt)>);
        REQUIRE(opt);
        REQUIRE(*opt == "text");
        opt = std::string("text2");
        REQUIRE(*opt == "text2");
        auto opt2 = opt;
        opt = {};
        REQUIRE(opt2 == "text2");
        REQUIRE(!opt);
        REQUIRE(opt.value_or("test") == "test");
        opt.swap(opt2);
        REQUIRE(opt == "text2");
        REQUIRE(!opt2);
    }
    {
        struct type {
            type(int a, std::string &&b)
                : a(a), b(std::move(b)) { }
            int a;
            std::string b;
        };
        auto opt = b0::make_optional<type>(1, "test");
        REQUIRE(opt);
        REQUIRE(opt->a == 1);
        REQUIRE(opt->b == "test");
        opt = {};
        REQUIRE(!opt);
    }
}
