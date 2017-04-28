#pragma once

#include "./_terminal_fn.hpp"

namespace b0 { namespace range {

template<class OutIt>
struct copy_fn : terminal_fn {
    copy_fn(OutIt oit): m_oit(std::move(oit)) {}

    template<class Iterable>
    auto run(Iterable &&it) const -> OutIt
    {
        auto oit = m_oit;
        for (; !it.at_end(); it.next(), ++oit)
            *oit = it.get();
        return oit;
    }
private:
    OutIt m_oit;
};

}}
