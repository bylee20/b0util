#include "path.hpp"
#include "./unicode.hpp"
#include "./meta/meta.hpp"

#ifdef B0_OS_WIN
#include <Windows.h>
#endif

namespace b0 {

auto app_file_path() -> std::string
{
#ifdef B0_OS_WIN
    wchar_t path[MAX_PATH + 1];
    const int len = static_cast<int>(GetModuleFileName(nullptr, path, MAX_PATH + 1));
    if (len >= MAX_PATH || len == 0)
        return {};
    for (int i = 0; i < len; ++i) {
        if (path[i] == L'\\')
            path[i] = L'/';
    }
    return to_utf8(path, len);
#endif
}

auto app_dir_path() -> std::string
{
    auto path = app_file_path();
    auto sep = path.find_last_of('/');
    return path.substr(0, sep);
}

auto app_dir_path(const std::string &file) -> std::string
{
    auto path = app_dir_path();
    path.reserve(path.size() + 1 + file.size());
    path += '/';
    path += file;
    return path;
}

}
