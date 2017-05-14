//#include <catch.hpp>
//#include "../src/algorithm.hpp"
////#include "../timing.hpp"
////#include "../pipe.hpp"
////#include <iostream>
//#include <deque>

//using namespace b0;

//template<class T>
//auto get(T &&t) -> decltype(auto)
//{ return forward_like<T>(t, t.a); }

//TEST_CASE("algorithm") {
//    struct T { int a; };
//    const T ct{}; T t{};
//    META_ASSERT(meta::eq_v<decltype(get(ct)), const int&>);
//    META_ASSERT(meta::eq_v<decltype(get(std::move(ct))), const int&&>);
//    META_ASSERT(meta::eq_v<decltype(get(t)), int&>);
//    META_ASSERT(meta::eq_v<decltype(get(std::move(t))), int&&>);

////    REQUIRE(flatten_as<std::vector<int>>(map(make_range(5), [] (auto &&v) { return make_range(v); })) == (std::vector<int>{0, 0, 1, 0, 1, 2, 0, 1, 2, 3}));
////    REQUIRE(flatten(map(make_range(5), [] (auto &&v) { return make_range(v); })) == (std::vector<int>{0, 0, 1, 0, 1, 2, 0, 1, 2, 3}));
////    {
////        auto range = b0::make_range(3);
////        auto f = [] (int v) { return std::to_string(v); };

////        REQUIRE(range == (std::vector<int>{0, 1, 2}));
////        REQUIRE(b0::map(range, f) == (std::vector<std::string>{"0", "1", "2"}));
////        REQUIRE(b0::map(b0::make_range(range.begin(), range.end()), f) == (std::vector<std::string>{"0", "1", "2"}));
////        REQUIRE(b0::map_as<std::deque<std::string>>(range, f) == (std::deque<std::string>{"0", "1", "2"}));
////        REQUIRE(b0::reduce(range, 0) == 3);
////        REQUIRE(b0::reduce(range, 0, [] (auto &&v) { return v * v; }) == 5);
////        REQUIRE(b0::reduce(range, 0, [] (auto &&acc, auto &&v) { return acc + v * v * v; }) == 9);
////        REQUIRE(b0::filter(range, [] (auto &&v) { return v % 2 == 0; }) == (std::vector<int>{0, 2}));
////        REQUIRE(b0::filter_as<std::deque<int>>(range, [] (auto &&v) { return v % 2 == 0; }) == (std::deque<int>{0, 2}));
////        REQUIRE(b0::clone(range) == range);
////        REQUIRE(b0::clone_as<std::deque<int>>(range) == (std::deque<int>{0, 1, 2}));
////    }
//}
