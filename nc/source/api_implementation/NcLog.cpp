#include "NcLog.h"
#include "log/Logger.h"

namespace nc::log
{
    void NcLogToDiagnostics(std::string item)
    {
        Logger::Log(std::move(item));
    }
}