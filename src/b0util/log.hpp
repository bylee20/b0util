#pragma once

#include <chrono>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
#include "global.hpp"

namespace b0 {

class logger_context;

enum class log_level {
    fatal = spdlog::level::critical,
    error = spdlog::level::err,
    warn = spdlog::level::warn,
    info = spdlog::level::info,
    debug = spdlog::level::debug,
    trace = spdlog::level::trace
};

class B0UTIL_API logger {
public:
    enum thread_policy { single_thread, multi_thread };
    enum console_sink { stdout_sink, stderr_sink };

    auto set_pattern(const std::string &pattern) -> void { m_log->set_pattern(pattern); }
    auto set_level(log_level lv) -> void { m_log->set_level(static_cast<spdlog::level::level_enum>(lv)); }

    auto flush() const -> void { m_log->flush(); }
    auto context(const std::string &name) const -> logger_context;
    auto context(std::string &&name) const -> logger_context;

    template<class... Args>
    auto info(const char *fmt, Args&&... args) const -> void { m_log->info(fmt, std::forward<Args>(args)...); }
    template<class... Args>
    auto error(const char *fmt, Args&&... args) const -> void { m_log->error(fmt, std::forward<Args>(args)...); }
    template<class... Args>
    auto warn(const char *fmt, Args&&... args) const -> void { m_log->warn(fmt, std::forward<Args>(args)...); }
    template<class... Args>
    auto debug(const char *fmt, Args&&... args) const -> void { m_log->debug(fmt, std::forward<Args>(args)...); }
    template<class... Args>
    auto trace(const char *fmt, Args&&... args) const -> void { m_log->trace(fmt, std::forward<Args>(args)...); }
    template<class... Args>
    auto fatal(const char *fmt, Args&&... args) const -> void { m_log->critical(fmt, std::forward<Args>(args)...); }

    static auto console(console_sink sink = stdout_sink, bool color = false) -> logger;
    static auto stdout_(bool color = false) -> logger;
    static auto stderr_(bool color = false) -> logger;
    static auto file(const std::string &filename) -> logger;
    static auto daily(const std::string &filename,
                      std::chrono::hours h = std::chrono::hours::zero(),
                      std::chrono::minutes min = std::chrono::minutes::zero()) -> logger;
    static auto rotating(const std::string &filename, int64_t size, int64_t count) -> logger;
    template<class... Args>
    static auto combine(Args&&... args) -> logger { return make_combined({ args.m_log.instance()... }); }
    static auto set_default(const logger &default_) -> void { logger::default_() = default_; }
    static auto set_default(logger &&default_) -> void { logger::default_() = std::move(default_); }
    static auto default_() -> logger& { static logger s_default; return s_default; }
private:
    friend class logger_context;
    static auto make_combined(std::initializer_list<spdlog::sink_ptr> loggers) -> logger;
    logger() {}
    logger(std::shared_ptr<spdlog::logger> log): m_log(std::move(log)) { }
    std::shared_ptr<spdlog::logger> m_log;
};

inline auto set_default_logger(const logger &logger) -> void { b0::logger::set_default(logger); }

inline auto default_logger() -> logger { return b0::logger::default_(); }

template<class... Args>
inline auto info(const char *fmt, Args&&... args) -> void
{ default_logger().info(fmt, std::forward<Args>(args)...); }

template<class... Args>
inline auto error(const char *fmt, Args&&... args) -> void
{ default_logger().error(fmt, std::forward<Args>(args)...); }

template<class... Args>
inline auto warn(const char *fmt, Args&&... args) -> void
{ default_logger().warn(fmt, std::forward<Args>(args)...); }

template<class... Args>
inline auto debug(const char *fmt, Args&&... args) -> void
{ default_logger().debug(fmt, std::forward<Args>(args)...); }

template<class... Args>
inline auto trace(const char *fmt, Args&&... args) -> void
{ default_logger().trace(fmt, std::forward<Args>(args)...); }

template<class... Args>
inline auto fatal(const char *fmt, Args&&... args) -> void
{ default_logger().fatal(fmt, std::forward<Args>(args)...); }

class logger_context {
public:
    logger_context(const std::string &name, const logger &logger)
        : m_name(name), m_logger(logger) {}
    logger_context(std::string &&name, const logger &logger)
        : m_name(std::move(name)), m_logger(logger) {}
    logger_context(const std::string &pattern, const std::string &separator, std::string &&name, const logger &logger)
        : m_pattern(pattern), m_sep(separator), m_name(std::move(name)), m_logger(logger) {}

    template<class Fmt, class... Args>
    auto fatal(Fmt &&fmt, Args&&... args) const -> void
    { log(log_level::fatal, std::forward<Fmt>(fmt), std::forward<Args>(args)...); }
    template<class Fmt, class... Args>
    auto error(Fmt &&fmt, Args&&... args) const -> void
    { log(log_level::error, std::forward<Fmt>(fmt), std::forward<Args>(args)...); }
    template<class Fmt, class... Args>
    auto warn(Fmt &&fmt, Args&&... args) const -> void
    { log(log_level::warn, std::forward<Fmt>(fmt), std::forward<Args>(args)...); }
    template<class Fmt, class... Args>
    auto info(Fmt &&fmt, Args&&... args) const -> void
    { log(log_level::info, std::forward<Fmt>(fmt), std::forward<Args>(args)...); }
    template<class Fmt, class... Args>
    auto debug(Fmt &&fmt, Args&&... args) const -> void
    { log(log_level::debug, std::forward<Fmt>(fmt), std::forward<Args>(args)...); }
    template<class Fmt, class... Args>
    auto trace(Fmt &&fmt, Args&&... args) const -> void
    { log(log_level::trace, std::forward<Fmt>(fmt), std::forward<Args>(args)...); }
    template<class Fmt, class... Args>
    auto log(log_level level, Fmt &&fmt, Args&&... args) const -> void
    {
        fmt::MemoryWriter out;
        out.write(m_pattern, m_name);
        out.write(std::forward<Fmt>(fmt), std::forward<Args>(args)...);
        m_logger.m_log->log(static_cast<spdlog::level::level_enum>(level), out.str());
    }
    auto context(const std::string &name) const -> logger_context
    {
        return { m_pattern, m_sep, fmt::format("{}{}{}", m_name, m_sep, name), m_logger };
    }
    auto name() const -> const std::string& { return m_name; }
    auto set_pattern(std::string pattern) -> void { m_pattern = std::move(pattern); }
    auto set_separator(std::string sep) -> void { m_sep = std::move(sep); }
    auto pattern() const -> const std::string& { return m_pattern; }
    auto separator() const -> const std::string& { return m_sep; }
private:
    B0UTIL_API static const std::string s_default_pattern;
    B0UTIL_API static const std::string s_default_separator;
    std::string m_pattern = s_default_pattern;
    std::string m_sep = s_default_separator;
    std::string m_name;
    logger m_logger;
};

inline auto log_context(const std::string &name) -> logger_context
{ return default_logger().context(name); }

}


