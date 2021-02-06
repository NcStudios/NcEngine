#pragma once

#include "platform/win32/NCWinDef.h"

#include <exception>
#include <iostream>

#define THROW_FAILED(hr) if ( (HRESULT)hr < 0 ) { throw DX11Exception(hr, __FILE__, __LINE__); }

namespace nc::graphics
{
    class DX11Exception : public std::exception
    {
        public:
            DX11Exception(HRESULT hr, const char* fileName, const int lineNumber) : result(hr), file(fileName), line(lineNumber) {}
            virtual const char* what() const throw() override
            {
                static char s_str[64] = {};
                std::cerr << s_str << "Big Fail!\n"
                                   << "    HRESULT: " << static_cast<unsigned int>(result) << "\n"
                                   << "    File: " << file << '\n'
                                   << "    Line: " << line << '\n';
                return s_str;
            }
        private:
            HRESULT result;
            const char* file;
            const int line;
    };
}