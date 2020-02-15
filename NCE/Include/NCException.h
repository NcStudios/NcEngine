#ifndef NC_EXCEPTION_H
#define NC_EXCEPTION_H

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
}

#endif