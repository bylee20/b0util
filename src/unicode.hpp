#pragma once

#include <string>

namespace b0 {

auto to_utf8(const std::wstring &data) -> std::string;
auto from_utf8(const std::string &data) -> std::wstring;

auto to_utf8(const wchar_t *data, int len) -> std::string;
auto from_utf8(const char *data, int len) -> std::wstring;

}
