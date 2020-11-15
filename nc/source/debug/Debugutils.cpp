#include "DebugUtils.h"

namespace nc::debug
{
    std::function<void(std::string)> Log::LogToDiagnostics_ = nullptr;

    void Log::LogToDiagnostics(std::string item)
    {
        Log::LogToDiagnostics_(std::move(item));
    }
}