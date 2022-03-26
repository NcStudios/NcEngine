#pragma once

#include "platform/SourceLocation.h"

#include <sstream>
#include <stdexcept>

namespace nc
{
    class NcError : public std::runtime_error
    {
        public:
            NcError(std::string msg, NC_SOURCE_LOCATION location = NC_SOURCE_LOCATION_CURRENT)
                : std::runtime_error(msg),
                  m_message{}
            {
                std::ostringstream os;
                os << "File: " << location.file_name() << '\n'
                   << "Func: " << location.function_name() << '\n'
                   << "Line: " << location.line() << '\n'
                   << "  " << msg;
                m_message = os.str();
            }

            const char* what() const noexcept override
            {
                return m_message.c_str();
            }

        private:
            std::string m_message;
    };
}