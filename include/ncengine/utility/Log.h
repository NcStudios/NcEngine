/**
 * @file Log.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "detail/LogInternal.h"

#include <string_view>

namespace nc
{
/** @brief Type of a log message. */
enum class LogCategory : char
{
    Info    = 'I',
    Warning = 'W',
    Error   = 'E',
    Verbose = 'V'
};

/** @brief Function type for receiving logging messages. */
using LogCallback_t = void(*)(LogCategory category,
                              std::string_view subsystem,
                              std::string_view function,
                              int line,
                              std::string_view message);

/** @brief Set a callback to reroute logging messages (defaults to stdout). */
void SetLogCallback(LogCallback_t callback);

/** @cond internal */
namespace detail
{
extern LogCallback_t LogCallback;

void DefaultLogCallback(LogCategory category,
                        std::string_view subsystem,
                        std::string_view function,
                        int line,
                        std::string_view message);
} // namespace detail
/** @endcond internal */
} // namespace nc

#if NC_LOG_LEVEL >= 1
    #define NC_LOG_INFO(str, ...)         nc::detail::LogCallback(nc::LogCategory::Info,    NC_LOG_CAPTURE_DEFAULT_ARGS(str NC_OPT_EXPAND(__VA_ARGS__)));
    #define NC_LOG_WARNING(str, ...)      nc::detail::LogCallback(nc::LogCategory::Warning, NC_LOG_CAPTURE_DEFAULT_ARGS(str NC_OPT_EXPAND(__VA_ARGS__)));
    #define NC_LOG_ERROR(str, ...)        nc::detail::LogCallback(nc::LogCategory::Error,   NC_LOG_CAPTURE_DEFAULT_ARGS(str NC_OPT_EXPAND(__VA_ARGS__)));
    #define NC_LOG_EXCEPTION(exception)   nc::detail::LogException(exception);

    #define NC_LOG_INFO_EXT(subsystem, file, line, str)    nc::detail::LogCallback(nc::LogCategory::Info, subsystem, file, line, str);
    #define NC_LOG_WARNING_EXT(subsystem, file, line, str) nc::detail::LogCallback(nc::LogCategory::Warning, subsystem, file, line, str);
    #define NC_LOG_ERROR_EXT(subsystem, file, line, str)   nc::detail::LogCallback(nc::LogCategory::Error, subsystem, file, line, str);
#else
    #define NC_LOG_INFO(str, ...);
    #define NC_LOG_WARNING(str, ...);
    #define NC_LOG_ERROR(str, ...);
    #define NC_LOG_EXCEPTION(exception);

    #define NC_LOG_INFO_EXT(subsystem, file, line, str);
    #define NC_LOG_WARNING_EXT(subsystem, file, line, str);
    #define NC_LOG_ERROR_EXT(subsystem, file, line, str);
#endif

#if NC_LOG_LEVEL >= 2
    #define NC_LOG_TRACE(str, ...)                         nc::detail::LogCallback(nc::LogCategory::Verbose, NC_LOG_CAPTURE_DEFAULT_ARGS(str NC_OPT_EXPAND(__VA_ARGS__)));
    #define NC_LOG_TRACE_EXT(subsystem, file, line, str)   nc::detail::LogCallback(nc::LogCategory::Verbose, subsystem, file, line, str);
#else
    #define NC_LOG_TRACE(str, ...);
    #define NC_LOG_TRACE_EXT(subsystem, file, line, str);
#endif
