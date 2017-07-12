#include "log.hpp"
#include "unicode.hpp"
#include <string>
#include <spdlog/spdlog.h>
#include <atomic>

namespace b0 {

static std::atomic_int32_t s_number;

static const auto s_pattern = [] () {
    spdlog::set_pattern("(%L)[%Y-%m-%d %H:%M:%S.%e] %v");
    return 0;
}();

static inline auto get_logger_name() -> std::string
{
    using namespace std::literals::string_literals;
    auto &&name = "b0_logger_"s;
    return name += std::to_string(s_number.load());
}

auto logger::console(console_sink sink, bool color) -> logger
{
    return sink == stdout_sink ? stdout_(color) : stderr_(color);
}

auto logger::stdout_(bool color) -> logger
{
    if (color)
        return spdlog::stdout_color_mt(get_logger_name());
    else
        return spdlog::stdout_logger_mt(get_logger_name());
}

auto logger::stderr_(bool color) -> logger
{
    if (color)
        return spdlog::stderr_color_mt(get_logger_name());
    else
        return spdlog::stderr_logger_mt(get_logger_name());
}

auto logger::file(const std::string &filename) -> logger
{
    return spdlog::basic_logger_mt(get_logger_name(), filename);
}

auto logger::daily(const std::string &filename, std::chrono::hours h, std::chrono::minutes min) -> logger
{
    return spdlog::daily_logger_mt(get_logger_name(), filename, h.count(), min.count());
}

auto logger::rotating(const std::string &filename, int64_t size, int64_t count) -> logger
{
    return spdlog::rotating_logger_mt(get_logger_name(), filename,
                                      static_cast<uint64_t>(size),
                                      static_cast<uint64_t>(count));
}

auto logger::make_combined(std::initializer_list<spdlog::sink_ptr> loggers) -> logger
{
    return std::make_shared<spdlog::logger>(get_logger_name(), loggers.begin(), loggers.end());
}

auto logger::context(const std::string &name) const -> logger_context
{
    return logger_context(name, *this);
}

auto logger::context(std::string &&name) const -> logger_context
{
    return logger_context(std::move(name), *this);
}

const std::string logger_context::s_default_pattern = "[{}] ";
const std::string logger_context::s_default_separator = "/";

}
