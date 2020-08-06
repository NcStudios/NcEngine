#pragma once
#include <stdexcept>

namespace nc
{
    #ifdef DEBUG_BUILD
    inline void IfThrow_(bool expr, const char* msg)
    {
        if(expr)
            throw std::runtime_error(msg);
    }

    #define IF_THROW(expr, msg) IfThrow_(expr, msg)

    #else
    #define IF_THROW(expr, msg)
    #endif
}