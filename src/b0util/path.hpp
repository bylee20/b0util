#pragma once

#include "./global.hpp"

namespace b0 {

B0UTIL_API auto app_file_path() -> std::string;
B0UTIL_API auto app_dir_path() -> std::string;
B0UTIL_API auto app_dir_path(const std::string &file) -> std::string;

constexpr inline auto separator() -> char
{
#ifdef Q_OS_WIN
    return '\\';
#else
    return '/';
#endif
}

}
