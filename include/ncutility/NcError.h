/**
 * @file NcError.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "platform/SourceLocation.h"

#include "fmt/format.h"

#include <stdexcept>
#include <string>

namespace nc
{
/** @brief Common exception type used throughout NcEngine. */
class NcError : public std::runtime_error
{
    public:
        NcError(std::string msg, NC_SOURCE_LOCATION location = NC_SOURCE_LOCATION_CURRENT)
            : std::runtime_error(msg),
              m_message{Format(msg, location)}
        {
        }

        NcError(std::string msg, std::string detail, NC_SOURCE_LOCATION location = NC_SOURCE_LOCATION_CURRENT)
            : std::runtime_error(msg),
              m_message{Format(msg, detail, location)}
        {
        }

        const char* what() const noexcept override
        {
            return m_message.c_str();
        }

    private:
        std::string m_message;

    auto Format(const std::string& msg, const NC_SOURCE_LOCATION& location) -> std::string
    {
        return fmt::format("File: {}\nFunc: {}\nLine: {}\n  {}\n",
            location.file_name(), location.function_name(), location.line(), msg);
    }

    auto Format(const std::string& msg, const std::string& detail, const NC_SOURCE_LOCATION& location) -> std::string
    {
        return fmt::format("File: {}\nFunc: {}\nLine: {}\n  {}\n  {}\n",
            location.file_name(), location.function_name(), location.line(), msg, detail);
    }
};
} // namespace nc

#ifdef NC_ASSERT_ENABLED
/** @brief Conditionally throw only in dev builds. */
#define NC_ASSERT(expr, msg) if(!(expr)) throw nc::NcError(msg);
#else
#define NC_ASSERT(expr, msg)
#endif
