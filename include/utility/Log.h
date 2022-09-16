#pragma once

#include "detail/LogInternal.h"

#include <format>

#if NC_LOG_LEVEL >= 1
    #define NC_LOG_INFO(fmt, ...)       nc::utility::detail::Log(std::format(fmt, __VA_ARGS__), 'I');
    #define NC_LOG_WARNING(fmt, ...)    nc::utility::detail::Log(std::format(fmt, __VA_ARGS__), 'W');
    #define NC_LOG_ERROR(fmt, ...)      nc::utility::detail::Log(std::format(fmt, __VA_ARGS__), 'E');
    #define NC_LOG_EXCEPTION(exception) nc::utility::detail::Log(exception);
#else
    #define NC_LOG_EXCEPTION(exception);
    #define NC_LOG_ERROR(fmt, ...);
    #define NC_LOG_WARNING(fmt, ...);
    #define NC_LOG_INFO(fmt, ...);
#endif

#if NC_LOG_LEVEL >= 2
    #define NC_LOG_TRACE(fmt, ...)      nc::utility::detail::Log(std::format(fmt, __VA_ARGS__), 'V');
#else
    #define NC_LOG_TRACE(fmt, ...);
#endif
