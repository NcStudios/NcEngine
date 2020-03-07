#pragma once
#include <exception>
#include <iostream>

namespace nc::graphics::internal
{
    class com_exception : public std::exception
    {
        public:
            com_exception(HRESULT hr, const char* fileName, const int lineNumber) : result(hr), file(fileName), line(lineNumber) {}
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


    inline void ThrowIfFailed(HRESULT hr, const char* file, const int line)
    {
        if(FAILED(hr))
        {
            throw com_exception(hr, file, line);
        }
    }
}