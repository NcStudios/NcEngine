#pragma once
#include <exception>

namespace nc
{
    class DefaultException : public std::exception
    {
        public:
            DefaultException(const char* msg) : message(msg) {}
            const char* what() const throw() { return message; }
        private:
            const char* message;
    };

    inline void IfThrow_(bool expr, const char* msg)
    {
        if(expr)
            throw DefaultException(msg);
    }

    #ifdef NC_EDITOR_ENABLED
    #define IF_THROW(expr, msg) IfThrow_(expr, msg)
    #else
    #define IF_THROW(expr, msg)
    #endif
}