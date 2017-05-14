#pragma once

#include <chrono>

namespace b0 {

template<class F>
auto timing(F &&f) -> auto
{
    using namespace std::chrono;
    const auto before = steady_clock::now();
    std::forward<F>(f)();
    const auto end = steady_clock::now();
    return end - before;
}

template<class F>
auto timing(F &&f, int repeat, bool avg = true) -> auto
{
    using namespace std::chrono;
    const auto before = steady_clock::now();
    for (int i = 0; i < repeat; ++i)
        std::forward<F>(f)();
    const auto end = steady_clock::now();
    const auto diff = end - before;
    return avg ? (diff / static_cast<double>(repeat)) : diff;
}

}
