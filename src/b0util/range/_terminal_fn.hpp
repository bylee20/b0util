#pragma once

#include "./pipe.hpp"

namespace b0 { namespace range {

struct terminal_fn {
    using run_policy = run_seq_t;
    constexpr terminal_fn() = default;
};

}}
