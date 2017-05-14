#pragma once

#ifdef _WIN64
#   define B0_OS_WIN64
#elif defined(_WIN32)
#   define B0_OS_WIN32
#endif

#ifdef __linux__
#   define B0_OS_LINUX
#   define B0_OS B0_OS_LINUX
#endif

#if (defined(B0_OS_WIN64) || defined(B0_OS_WIN32))
#   define B0_OS_WIN
#   define B0_OS B0_OS_WIN
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#define B0_DEP_TMP
#else
#define B0_DEP_TMP template
#endif

#ifdef _MSC_VER
#   define B0_DECL_EXPORT __declspec(dllexport)
#   define B0_DECL_IMPORT __declspec(dllimport)
#else
#   define B0_DECL_EXPORT
#   define B0_DECL_IMPORT
#endif

#define B0_SI static inline
#define B0_SC static constexpr
#define B0_SCI static constexpr inline
#define B0_CI constexpr inline

#if defined(B0UTIL_LIBRARY)
#   define B0UTIL_API B0_DECL_EXPORT
#else
#   define B0UTIL_API B0_DECL_IMPORT
#endif

#ifdef QT_CORE_LIB

#include <QtCore/QString>
#include <QtCore/QStringBuilder>

inline auto operator "" _q(const char16_t *data, std::size_t len) -> QString
{
    static_assert(sizeof(QChar) == sizeof(char16_t), "!!!");
    return QString::fromRawData(reinterpret_cast<const QChar*>(data), static_cast<int>(len));
}

inline auto operator "" _q(const char *data, std::size_t len) -> QString
{ return QString::fromLatin1(data, static_cast<int>(len)); }

inline auto operator "" _l1(const char *data, std::size_t len) -> QLatin1String
{ return QLatin1String(data, static_cast<int>(len)); }

#endif

#define B0_DEFAULT_COPY_CTOR(type) type(const type&) = default
#define B0_DEFAULT_MOVE_CTOR(type) type(type&&) = default

#define B0_DEFAULT_COPY_ASSIGN(type) auto operator = (const type&) -> type& = default
#define B0_DEFAULT_MOVE_ASSIGN(type) auto operator = (type&&) -> type& = default

#define B0_DEFAULT_COPY_MOVE_CTOR(type) \
    B0_DEFAULT_COPY_CTOR(type); \
    B0_DEFAULT_MOVE_CTOR(type)

#define B0_DEFAULT_COPY_MOVE_ASSIGN(type) \
    B0_DEFAULT_COPY_ASSIGN(type); \
    B0_DEFAULT_MOVE_ASSIGN(type)

#define B0_DEFAULT_COPY_MOVE(type) \
    B0_DEFAULT_COPY_MOVE_CTOR(type); \
    B0_DEFAULT_COPY_MOVE_ASSIGN(type)

#define B0_CONSTEXPR_DEFAULT_COPY_MOVE(type) \
    constexpr type(const type&) = default; \
    constexpr type(type&&) = default; \
    auto operator = (const type&) -> type& = default; \
    auto operator = (type&&) -> type& = default

namespace b0 {

using expander = int[];

#define b0_expand(...) ::b0::expander{((void)__VA_ARGS__, 0)...}

#define b0_autotype(expr) std::decay_t<decltype(expr)>

}
