#pragma once

#include "ncutility/platform/SourceLocation.h"

#include "fmt/format.h"

#include <exception>
#include <utility>

namespace nc::detail
{
void LogException(const std::exception& e) noexcept;

constexpr auto TrimToFilename(const char* path) noexcept -> const char*
{
    constexpr auto separator =
        #if defined(_WIN32) && ! defined(__MINGW32__)
            '\\';
        #else
            '/';
        #endif

    auto file = path;
    while (*path)
    {
        if (*path++ == separator)
            file = path;
    }

    return file;
}
} // namespace nc::detail

#define NC_LOG_CAPTURE_DEFAULT_ARGS(...) \
"NcEngine", nc::detail::TrimToFilename(NC_SOURCE_FILE), NC_SOURCE_LINE, fmt::format(__VA_ARGS__)

#define NC_LOG_FMT_MSG(category, subsystem, file, line, msg)                                       \
(file.empty()                                                                                      \
    ? fmt::format("{} [{}] {}\n", std::to_underlying(category), subsystem, msg)                    \
    : fmt::format("{} [{}] {}:{}: {}\n", std::to_underlying(category), subsystem, file, line, msg) \
)

#define NC_VA_OPT_SUPPORTED_SELECT(a,b,c,...) c
#define NC_VA_OPT_SUPPORTED_IMPL(...) NC_VA_OPT_SUPPORTED_SELECT(__VA_OPT__(,),true,false,)
#define NC_VA_OPT_SUPPORTED NC_VA_OPT_SUPPORTED_IMPL(?)

#if NC_VA_OPT_SUPPORTED
    #define NC_OPT_EXPAND(...) __VA_OPT__(,) __VA_ARGS__
#elif defined(__GNUC__)
    #define NC_OPT_EXPAND(...) , ##__VA_ARGS__
#else
    #define NC_OPT_EXPAND(...) , __VA_ARGS__
#endif
