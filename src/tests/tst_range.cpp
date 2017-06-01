#include <b0util/range.hpp>
#include <deque>
#include <list>
#include <catch/catch.hpp>

TEST_CASE("range") {
    using namespace b0::range;
    using namespace std::chrono_literals;

    SECTION("flatten") {
        auto vvec = counter(0) | limit(3) | map([] (auto v) { return repeat(v, v) | to<std::vector>(); }) | to<std::vector>();
        CAPTURE(vvec);
        REQUIRE(vvec == (std::vector<std::vector<int>>{{}, {1}, {2, 2}}));
        auto vec = vvec | flatten() | map([] (auto v) { return v + 1; }) | to<std::vector>();
        REQUIRE(vec == (std::vector<int>{2, 3, 3}));
        vec = counter(0) | limit(3) | map([] (auto v) { return repeat(v, v) | to<std::vector>(); })
                | flatten() | map([] (auto v) { return v + 1; }) | to<std::vector>();
        REQUIRE(vec == (std::vector<int>{2, 3, 3}));
    }

    SECTION("counter/limit/to/filter/count/count_if") {
        std::vector<int> v1 = counter(0, 2) | limit(10) | to<std::vector>();
        std::vector<int> v2 = counter(0) | filter([] (int v) { return v % 2 == 0; })
                | limit(10) | to<std::vector>();
        REQUIRE(v1 == v2);
        REQUIRE((v1 | count()) == 10);
        REQUIRE((v2 | count()) == 10);
        REQUIRE((v1 | count_if([] (auto v) { return v % 2 == 1; })) == 0);
    }

    SECTION("map/reduce/sum") {
        auto r1 = counter(0) | limit(10) | map([] (auto v) { return v * v; }) | sum();
        auto r2 = counter(0) | map([] (auto v) { return v * v; }) | limit(10)
                | reduce(0, [] (auto &&acc, auto v) { acc += v; });
        REQUIRE(r1 == r2);
        REQUIRE(r1 == 285);

        std::vector<double> vec = {1.0, 5.0, 2.0, 0.0};
        REQUIRE((vec | max()) == 5.0);
        REQUIRE((vec | min()) == 0.0);
        REQUIRE((vec | minmax()) == std::make_pair(0.0, 5.0));

        REQUIRE(map_to<std::vector>(counter(0) | limit(3) | to<std::vector>(), [] (auto v) { return v + 1; }) == (std::vector<int>{1, 2, 3}));
    }

    SECTION("find/find_if") {
        auto deque = counter(0) | limit(10) | to<std::deque>();
        REQUIRE(((deque | find(5)) - deque.begin()) == 5);
        REQUIRE(((deque | map([] (auto v) { return v + 1; }) | find_if([] (auto v) { return v % 7 == 0; })) - deque.begin()) == 6);
        REQUIRE((deque | find_if([] (auto v) { return v < 0; })) == deque.end());
    }

    SECTION("for_each") {
        auto vector = counter(0) | limit(10) | to<std::vector>();
        view(vector) | for_each([] (auto &&v) { v *= 2; });
        REQUIRE(vector == (counter(0, 2) | limit(10) | to<std::vector>()));
    }

    SECTION("filter-ref") {
        auto v = counter(0) | limit(5) | to<std::vector>();
        v | filter([] (auto &&v) { return v % 2 == 0; }) | for_each([] (auto &v) { v += 10; });
        REQUIRE(v == (std::vector<int>{10, 1, 12, 3, 14}));
    }


    SECTION("min/max/find_min/find_max") {
        std::vector<double> vec;
        CAPTURE(vec);
        REQUIRE((vec | find_min()) == vec.end());
        REQUIRE((vec | find_max()) == vec.end());
        vec = {1.0, 5.0, 2.0, 0.0};
        CAPTURE(vec);
        REQUIRE(*(vec | find_min()) == 0.0);
        REQUIRE(*(vec | find_max()) == 5.0);
        REQUIRE((vec | min()) == 0.0);
        REQUIRE((vec | max()) == 5.0);
        auto minmax = vec | find_minmax();
        REQUIRE(*minmax.first == 0.0);
        REQUIRE(*minmax.second == 5.0);
        REQUIRE(std::distance(vec.begin(), minmax.first) == 3);
        REQUIRE(std::distance(vec.begin(), minmax.second) == 1);
    }

    SECTION("parallel") {
        auto input = counter(0) | limit(10) | to<std::vector>();
        auto output = input | map([] (auto v) { return v + 1; }) | to<std::vector>(b0::run_par_sync);
        REQUIRE(output == (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));
    }
}
