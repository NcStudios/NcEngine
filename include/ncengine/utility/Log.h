#pragma once

#include "detail/LogInternal.h"

#include "fmt/format.h"

#if NC_LOG_LEVEL >= 1
    #define NC_LOG_INFO(str)             nc::utility::detail::Log(str, 'I');
    #define NC_LOG_INFO_FMT(str, ...)    nc::utility::detail::Log(fmt::format(str, __VA_ARGS__), 'I');
    #define NC_LOG_WARNING(str)          nc::utility::detail::Log(str, 'W');
    #define NC_LOG_WARNING_FMT(str, ...) nc::utility::detail::Log(fmt::format(str, __VA_ARGS__), 'W');
    #define NC_LOG_ERROR(str)            nc::utility::detail::Log(str, 'E');
    #define NC_LOG_ERROR_FMT(str, ...)   nc::utility::detail::Log(fmt::format(str, __VA_ARGS__), 'E');
    #define NC_LOG_EXCEPTION(exception)  nc::utility::detail::Log(exception);
#else
    #define NC_LOG_INFO(str);
    #define NC_LOG_INFO_FMT(str, ...);
    #define NC_LOG_WARNING(str);
    #define NC_LOG_WARNING_FMT(str, ...);
    #define NC_LOG_ERROR(str);
    #define NC_LOG_ERROR_FMT(str, ...);
    #define NC_LOG_EXCEPTION(exception);
#endif

#if NC_LOG_LEVEL >= 2
    #define NC_LOG_TRACE(str)          nc::utility::detail::Log(str, 'V');
    #define NC_LOG_TRACE_FMT(str, ...) nc::utility::detail::Log(fmt::format(str, __VA_ARGS__), 'V');
#else
    #define NC_LOG_TRACE(str);
    #define NC_LOG_TRACE_FMT(str, ...);
#endif
