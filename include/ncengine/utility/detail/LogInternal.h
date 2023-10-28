#pragma once

#include "ncutility/platform/SourceLocation.h"

#include <string_view>

namespace nc
{
namespace config
{
struct ProjectSettings;
} // namespace config

namespace utility::detail
{
void InitializeLog(const config::ProjectSettings& settings);
void CloseLog() noexcept;
void Log(char prefix, std::string_view item) noexcept;
void Log(char prefix, std::string_view file, int line, std::string_view item) noexcept;
void Log(const std::exception& e) noexcept;

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
} // namespace utility::detail
} // namespace nc

#define NC_LOG_IMPL(prefix, ...)                                              \
nc::utility::detail::Log(prefix,                                              \
                         nc::utility::detail::TrimToFilename(NC_SOURCE_FILE), \
                         NC_SOURCE_LINE,                                      \
                         fmt::format(__VA_ARGS__));

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
