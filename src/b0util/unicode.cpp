#include "unicode.hpp"
#include "global.hpp"

#ifdef B0_OS_WIN
#include <Windows.h>
#endif

namespace b0 {

//template<class cvt>
//static auto converter() -> auto&
//{ thread_local std::wstring_convert<cvt, typename cvt::intern_type> conv; return conv; }

//#ifdef _MSC_VER
//static auto convUtf8Utf16() -> auto&
//{ return converter<std::codecvt_utf8_utf16<int16_t>>(); }
//static auto convUtf8Utf32() -> auto&
//{ return converter<std::codecvt_utf8<int32_t>>(); }
//inline auto toUtf8(const int16_t *data, int len) -> std::string
//{ return convUtf8Utf16().to_bytes(data, data + len); }
//inline auto toUtf8(const int32_t *data, int len) -> std::string
//{ return convUtf8Utf32().to_bytes(data, data + len); }
//inline auto toUtf8(const char16_t *data, int len) -> std::string
//{ return toUtf8(reinterpret_cast<const int16_t*>(data), len); }
//inline auto toUtf8(const char32_t *data, int len) -> std::string
//{ return toUtf8(reinterpret_cast<const int32_t*>(data), len); }
//#else
//inline auto convUtf8Utf16() -> auto&
//{ return converter<std::codecvt_utf8_utf16<char16_t>>(); }
//inline auto convUtf8Utf32() -> auto&
//{ return converter<std::codecvt_utf8<char32_t>>(); }
//inline auto toUtf8(const char16_t *data, int len) -> std::string
//{ return convUtf8Utf16().to_bytes(data, data + len); }
//inline auto toUtf8(const char32_t *data, int len) -> std::string
//{ return convUtf8Utf32().to_bytes(data, data + len); }
//#endif

auto to_utf8(const wchar_t *data, int len) -> std::string
{
#ifdef B0_OS_WIN
    const auto len8 = WideCharToMultiByte(CP_UTF8, 0, data, len, nullptr, 0, nullptr, nullptr);
    if (!len8)
        return {};
    std::string utf8; utf8.resize(static_cast<std::string::size_type>(len8));
    WideCharToMultiByte(CP_UTF8, 0, data, len, &utf8[0], len8, nullptr, nullptr);
    return utf8;
#endif
}

auto from_utf8(const char *data, int len) -> std::wstring
{
#ifdef B0_OS_WIN
    const auto wlen = MultiByteToWideChar(CP_UTF8, 0, data, len, NULL, NULL);
    if (!wlen)
        return {};
    std::wstring wstr; wstr.resize(static_cast<std::string::size_type>(wlen));
    MultiByteToWideChar(CP_UTF8, 0, data, len, &wstr[0], wlen);
    return wstr;
#endif
}

}
