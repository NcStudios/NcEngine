#pragma once
#include <exception>
#include <iostream>

namespace nc::graphics::internal
{
    class com_exception : public std::exception
    {
        public:
            com_exception(HRESULT hr) : result(hr) {}
            virtual const char* what() const throw() override
            {
                static char s_str[64] = {};
                std::cout << s_str << " Failure with HRESULT: " << static_cast<unsigned int>(result) << std::endl;
                return s_str;
            }
        private:
            HRESULT result;
    };


    inline void ThrowIfFailed(HRESULT hr)
    {
        if(FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}