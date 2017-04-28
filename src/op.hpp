#pragma once

#include "global.hpp"
#include <utility>

#define B0_NOP_LEFT_OP <
#define B0_NOP_RIGHT_OP >
#define B0_NOP_RIGHT_OP_EQ >=

namespace b0 {

namespace detail {
template<class Func>
struct __op { Func func; };
}

template<class Func>
B0_SCI auto make_op(Func &&func) -> detail::__op<uncref_t<Func> >
{ return { std::forward<Func>(func) }; }

namespace detail {

template<class T, class Func>
struct __op_holder {
    static_assert(std::is_reference_v<T>, "!!!");
    T lhs; const Func &func;

    template<class R>
    auto operator B0_NOP_RIGHT_OP (R &&rhs) -> decltype(auto)
    { return func(std::forward<T>(lhs), std::forward<T>(rhs)); }
    template<class R>
    auto operator B0_NOP_RIGHT_OP_EQ (R &&rhs) -> decltype(auto)
    { lhs = (*this B0_NOP_RIGHT_OP std::forward<R>(rhs)); return lhs; }
};

template <class T, class Func>
B0_SI auto operator B0_NOP_LEFT_OP (T &&lhs, const __op<Func> &rhs)
-> __op_holder<decltype(std::forward<T>(lhs)), Func>
{ return { std::forward<T>(lhs), rhs.func }; }


}

}
