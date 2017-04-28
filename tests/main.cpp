#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

auto main(int argc, char** argv) -> int
{
    const auto result = Catch::Session().run(std::min(1, argc), argv);
    return (result < 0xff ? result : 0xff);
}
