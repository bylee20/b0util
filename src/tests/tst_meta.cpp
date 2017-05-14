#include <catch.hpp>
#include <iostream>
#include <b0util/meta/meta.hpp>
#include <b0util/meta/type_seq.hpp>
#include <b0util/meta/callable.hpp>
#include <QString>
#include <QStringBuilder>

template<class F>
struct FuncWrapper {
    template<class T>
    friend constexpr auto operator % (T &&lhs, FuncWrapper &&rhs) -> decltype(auto)
    { return std::move(rhs.func)(std::forward<T>(lhs)); }
    template<class T>
    friend constexpr auto operator % (T &&lhs, const FuncWrapper &rhs) -> decltype(auto)
    { return rhs.m_func(std::forward<T>(lhs)); }
    F m_func;
};

template<class F>
auto make_wrapper(F &&func) -> auto {
    return FuncWrapper<std::decay_t<F>>{std::forward<F>(func)};
}

auto get = [] (int i) { return make_wrapper([i] (auto &&s) { return s[i]; }); };

TEST_CASE("b0::meta") {
    using namespace b0::meta;

    META_ASSERT(eq_v<if_t<true, int, double>, int>);

    META_ASSERT(eq_v<variadic_type_at_t<0, int>, int>);
    META_ASSERT(eq_v<variadic_type_at_t<0, int, double>, int>);
    META_ASSERT(eq_v<variadic_type_at_t<1, int, double>, double>);
    META_ASSERT(eq_v<variadic_type_at_t<0, int, double, char>, int>);
    META_ASSERT(eq_v<variadic_type_at_t<1, int, double, char>, double>);
    META_ASSERT(eq_v<variadic_type_at_t<2, int, double, char>, char>);

    META_ASSERT(variadic_type_index_v<int, int> == 0);
    META_ASSERT(variadic_type_index_v<int, int, double> == 0);
    META_ASSERT(variadic_type_index_v<double, int, double> == 1);
    META_ASSERT(variadic_type_index_v<int, int, double, char> == 0);
    META_ASSERT(variadic_type_index_v<double, int, double, char> == 1);
    META_ASSERT(variadic_type_index_v<char, int, double, char> == 2);

    META_ASSERT(eq_v<get_t<0, type_seq<int>>, int>);
    META_ASSERT(eq_v<get_t<0, type_seq<int, double>>, int>);
    META_ASSERT(eq_v<get_t<1, type_seq<int, double>>, double>);
    META_ASSERT(eq_v<get_t<0, type_seq<int, double, char>>, int>);
    META_ASSERT(eq_v<get_t<1, type_seq<int, double, char>>, double>);
    META_ASSERT(eq_v<get_t<2, type_seq<int, double, char>>, char>);
    META_ASSERT(eq_v<get_t<0, type_seq<int, double, int>>, int>);

    META_ASSERT(type_index_v<int, type_seq<int>> == 0);
    META_ASSERT(type_index_v<double, type_seq<int, double>> == 1);
    META_ASSERT(type_index_v<char, type_seq<int, double, char>> == 2);
    META_ASSERT(type_index_v<double, type_seq<int, double, char, double>> == 1);

    META_ASSERT(size_v<type_seq<>> == 0);
    META_ASSERT(size_v<type_seq<int>> == 1);
    META_ASSERT(size_v<type_seq<int, double>> == 2);
    META_ASSERT(size_v<type_seq<int, double, char>> == 3);

    META_ASSERT(eq_v<pop_front_t<type_seq<int, double>>, type_seq<double>>);
    META_ASSERT(eq_v<type_seq<int, double, char, float>::extract<1, 3>, type_seq<double, float>>);
    META_ASSERT(eq_v<type_seq<int, double, char, float>::slice_front<2>, type_seq<int, double>>);
    META_ASSERT(eq_v<type_seq<int, double, char, float>::slice_back<3>, type_seq<double, char, float>>);
    META_ASSERT(eq_v<slice_t<3, 6, type_seq<int, double, char, float, short, long>>, type_seq<float, short, long>>);
    META_ASSERT(eq_v<slice_t<3, 3, type_seq<int, double, char, float, short, long>>, type_seq<>>);
    META_ASSERT(eq_v<slice_t<3, 4, type_seq<int, double, char, float, short, long>>, type_seq<float>>);
    META_ASSERT(eq_v<slice_t<1, 4, type_seq<int, double, char, float, short, long>>, type_seq<double, char, float>>);
    META_ASSERT(eq_v<slice_t<0, 5, type_seq<int, double, char, float, short, long>>, type_seq<int, double, char, float, short>>);

//    int a = 0;
//    auto lambda2 = [&] (int x, int y) { return a + x + y; };
//    auto lambda1 = [&] (int x) { return lambda2(x, 3); };
//    auto lambda0 = [&] () { return lambda1(4); };
//    auto glambda = [&] (auto x, auto y) { return a + x + y; };

//    using Lambda0 = decltype(lambda0);
//    using Lambda1 = decltype(lambda1);
//    using Lambda2 = decltype(lambda2);
//    using GLambda = decltype(glambda);

//    META_ASSERT(arity_v<Lambda0> == 0);
//    META_ASSERT(arity_v<Lambda1> == 1);
//    META_ASSERT(arity_v<Lambda2> == 2);

//    META_ASSERT(is_callable)
}
