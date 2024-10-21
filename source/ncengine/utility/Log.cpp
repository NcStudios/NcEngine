#include "ncengine/utility/Log.h"

#include <iostream>

namespace nc
{
void SetLogCallback(LogCallback_t callback)
{
    detail::LogCallback = callback;
}

namespace detail
{
LogCallback_t LogCallback = DefaultLogCallback;

void DefaultLogCallback(nc::LogCategory category,
                        std::string_view subsystem,
                        std::string_view function,
                        int line,
                        std::string_view message)
{
    std::cout << NC_LOG_FMT_MSG(category, subsystem, function, line, message);
}

void LogException(const std::exception& e) noexcept
{
    LogCallback(LogCategory::Error, "NcEngine", "", 0, fmt::format("***EXCEPTION***\n{}", e.what()));

    try
    {
        std::rethrow_if_nested(e);
    }
    catch(const std::exception& rethrown)
    {
        LogException(rethrown);
    }
}
} // namespace detail
} // namespace nc
