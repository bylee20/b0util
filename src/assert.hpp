//#pragma once

//#include <exception>

//namespace b0 {

//template<class T>
//class assert_wrapper {
//    constexpr assert_wrapper(const T &t, const char *expr, const char *file, int line, const char *func)
//        : m(t), m_expr(expr), m_file(file), m_line(line), m_func(func) { }
//    template<class U>
//    constexpr auto operator == (const U &u) const -> void {
//        if (m != u) {

//        }
//        return m == u;
//    }
//private:
//    const T& m;
//    const char *m_expr;
//    const char *m_file;
//    int m_line;
//    const char *m_func;
//};

//template<class T>
//auto make_assert_wrapper(const T &t, const char *expr, const char *file, int line) -> auto
//{
//    return assert_wrapper<T>(t, expr, file, line);
//}

//#define b0_assert(val) assert_wrapper<std::decay_t<decltype(val)>>(val, #val, __FILE__, __LINE__, __func__);

//}
