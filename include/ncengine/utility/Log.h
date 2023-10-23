/**
 * @file Log.h
 * @copyright Jaremie and McCallister Romer 2023
 */
#pragma once

#include "detail/LogInternal.h"

#include "fmt/format.h"

#if NC_LOG_LEVEL >= 1
    #define NC_LOG_INFO(str, ...)         NC_LOG_IMPL('I', str NC_OPT_EXPAND(__VA_ARGS__));
    #define NC_LOG_WARNING(str, ...)      NC_LOG_IMPL('W', str NC_OPT_EXPAND(__VA_ARGS__));
    #define NC_LOG_ERROR(str, ...)        NC_LOG_IMPL('E', str NC_OPT_EXPAND(__VA_ARGS__));
    #define NC_LOG_EXCEPTION(exception)   nc::utility::detail::Log(exception);
#else
    #define NC_LOG_INFO(str, ...);
    #define NC_LOG_WARNING(str, ...);
    #define NC_LOG_ERROR(str, ...);
    #define NC_LOG_EXCEPTION(exception);
#endif

#if NC_LOG_LEVEL >= 2
    #define NC_LOG_TRACE(str, ...)        NC_LOG_IMPL('V', str NC_OPT_EXPAND(__VA_ARGS__));
#else
    #define NC_LOG_TRACE(str, ...);
#endif
