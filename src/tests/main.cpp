#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <path.hpp>

auto main(int argc, char** argv) -> int
{
    std::cout << "path: " << b0::app_dir_path() << std::endl;
    const auto result = Catch::Session().run(std::min(1, argc), argv);
    return (result < 0xff ? result : 0xff);
}
