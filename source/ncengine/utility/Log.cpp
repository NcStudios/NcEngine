#include "ncengine/utility/Log.h"
#include "ncengine/config/Config.h"

#include "ncutility/NcError.h"

#include <iostream>

namespace
{
void ConsoleLog(nc::LogCategory category,
                std::string_view subsystem,
                std::string_view function,
                int line,
                std::string_view message)
{
    std::cout << NC_LOG_FMT_MSG(category, subsystem, function, line, message);
}
} // anonymous namespace

namespace nc
{
LogCallback_t LogCallback = ::ConsoleLog;

void SetLogCallback(LogCallback_t callback)
{
    LogCallback = callback;
}

namespace detail
{
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
