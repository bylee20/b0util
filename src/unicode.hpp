#pragma once

#include <string>
#include "./global.hpp"

namespace b0 {

B0UTIL_API auto to_utf8(const wchar_t *data, int len) -> std::string;
B0UTIL_API auto from_utf8(const char *data, int len) -> std::wstring;

inline auto to_utf8(const std::wstring &data) -> std::string
{ return to_utf8(data.data(), static_cast<int>(data.size())); }

inline auto from_utf8(const std::string &data) -> std::wstring
{ return from_utf8(data.data(), static_cast<int>(data.size())); }


}
