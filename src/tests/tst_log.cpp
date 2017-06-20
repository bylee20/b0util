#include <catch/catch.hpp>
#include <b0util/log.hpp>

struct CustomType {
    friend auto operator<<(std::ostream &os, const CustomType &c) -> std::ostream&{
        os << c.value;
        return os;
    }
    int value;
};

TEST_CASE("log") {
    auto logger = b0::logger::console();
    logger.set_level(b0::log_level::trace);
    auto ctx = logger.context("Log");
    ctx.set_pattern("({}) ");
    ctx.set_separator(":");
    ctx = ctx.context("sub");

    ctx.debug("{}", CustomType{10});
}
