//#include "rho.hpp"
//#include <iostream>
//#include <vector>
//#include <catch.hpp>

//namespace Catch {

//template<class Iter>
//auto print(std::ostringstream &out, Iter begin, Iter end) -> std::ostringstream&
//{
//    out << "{ ";
//    for (auto it = begin; it != end; ++it) {
//        out << *it;
//        if (it != end)
//            out << ", ";
//    }
//    out << " }";
//    return out;
//}

//template<class T>
//auto operator << (std::ostringstream &out, const std::vector<T> &vec) -> std::ostringstream&
//{ return print(out, vec.begin(), vec.end()); }
//template<class T>
//auto operator << (std::ostringstream &out, const QVector<T> &vec) -> std::ostringstream&
//{ return print(out, vec.begin(), vec.end()); }

//template<class T, int N>
//auto operator << (std::ostringstream &out, const T (&vec)[N]) -> std::ostringstream&
//{ return print(out, std::begin(vec), std::end(vec)); }

//}


//TEST_CASE("rho") {
//    SECTION("binaryFind") {
//        int vec[] = { 0, 1, 2, 4, 5 };
//        REQUIRE(b0::binaryFind(std::begin(vec), std::end(vec), -1) == std::end(vec));
//        REQUIRE(b0::binaryFind(std::begin(vec), std::end(vec), 3) == std::end(vec));
//        REQUIRE(b0::binaryFind(std::begin(vec), std::end(vec), 10) == std::end(vec));
//        REQUIRE(*b0::binaryFind(std::begin(vec), std::end(vec), 1) == 1);
//        REQUIRE(*b0::binaryFind(std::begin(vec), std::end(vec), 0) == 0);
//        REQUIRE(*b0::binaryFind(std::begin(vec), std::end(vec), 5) == 5);
//        REQUIRE(*b0::binaryFind(vec, 2) == 2);
//        REQUIRE(*b0::binaryFind(vec, 4, [] (int l, int r) { return l < r; }) == 4);
//    }
//    SECTION("map") {
//        REQUIRE(b0::map<QVector<int> >({1, 2, 3}, [] (int v) { return v * v; }) == (QVector<int>{1, 4, 9}));
//        REQUIRE(b0::mapAs<std::vector<int> >({1, 2, 3}, [] (int v) { return v * v; }) == (std::vector<int>{1, 4, 9}));
//        REQUIRE(b0::map<QVector<int> >(true, {1, 2, 3}, [] (int v) { return v * v; }) == (QVector<int>{1, 4, 9}));
//        REQUIRE(b0::mapAs<std::vector<int> >(true, {1, 2, 3}, [] (int v) { std::this_thread::sleep_for(std::chrono::milliseconds(10-v)); return v * v; }) == (std::vector<int>{1, 4, 9}));
//    }

////    SECTION("rho.sort") {
////        REQUIRE(rho.sorted(std::vector<int>({ 2, 1, 6, 12, 0, 3 })) == std::vector<int>({0, 1, 2, 3, 6, 12}));
////    }
//}
