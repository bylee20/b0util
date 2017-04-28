//#include <catch.hpp>
//#include <range.hpp>
//#include <concurrent.hpp>
//#include <thread_pool.hpp>

//auto test() {
//    auto f = [] (int i) { return i * 0.5;};
//    const auto result = b0::map_as<std::vector<double>>(b0::run_par_unseq, b0::view(3), f);
//}

//TEST_CASE("concurrent") {
//    {
//        const auto result = b0::async([] (int a) -> int { return a*a;}, 5);
//        REQUIRE(result.get() == 25);
//    }
//    {
//        b0::for_each(b0::run_par_unseq, b0::view(3), [] (int a) { return a * 0.5; }).get();
//        REQUIRE(b0::map_as<std::vector<double>>(b0::run_par_unseq, b0::view(3), [] (int a) { return a * 0.5; }).get() == (std::vector<double>{0, 0.5, 1 }));
//    }
////    REQUIRE(b0::filter_as<std::vector<int>>(b0::run_par_unseq, b0::make_range(10), [] (int a) { return a % 2; }).get() == (std::vector<int>{1, 3, 5, 7, 9}));
////    REQUIRE(b0::reduce(b0::run_par_unseq, b0::make_range(10), 0, b0::identity).get() == 45);
//}
