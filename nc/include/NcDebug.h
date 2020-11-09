#pragma once

#include "log/Logger.h"

#include <stdexcept>

#ifdef DEBUG_BUILD
    namespace nc::debug
    {
        inline void IfThrow_(bool expr, const char* msg)
        {
            if(expr)
                throw std::runtime_error(msg);
        }
    }
    #define IF_THROW(expr, msg) nc::debug::IfThrow_(expr, msg)
#else
    #define IF_THROW(expr, msg)
#endif

#ifdef VERBOSE_LOGGING_ENABLED
#define V_LOG(item); \
        nc::log::Logger::Log(item); \
        nc::log::Logger::Log(std::string("    Func: ") + __PRETTY_FUNCTION__); \
        nc::log::Logger::Log(std::string("    File: ") + __FILE__); \
        nc::log::Logger::Log(std::string("    Line: ") + std::to_string(__LINE__));
#else
    #define V_LOG(item);
#endif
