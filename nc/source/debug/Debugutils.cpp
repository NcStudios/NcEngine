#include "DebugUtils.h"

namespace nc::debug
{
    std::function<void(std::string)> Log::LogToDiagnostics_ = nullptr;

    void Log::LogToDiagnostics(std::string item)
    {
        IF_THROW(!Log::LogToDiagnostics_, "Log::LogToDiagnostics_ is not bound");
        Log::LogToDiagnostics_(std::move(item));
    }
}