#pragma once

#include "NcError.h"

#include <string>

namespace nc::debug
{
    /** Log an item to the diagnostics file specified in config.ini */
    void LogToDiagnostics(const std::string& item) noexcept;

    /** Log an exception(or exceptions if nested) to the diagnostics file 
     *  specified in config.ini. If logging is not possible, results are sent
     *  to std::cerr */
    void LogException(const std::exception& e) noexcept;

    namespace internal
    {
        void OpenLog(const std::string& path);
        void CloseLog() noexcept;
    }
}

#ifdef NC_DEBUG_BUILD
/** Conditionally throw only in debug builds. */
#define IF_THROW(expr, msg) if(expr) throw nc::NcError(msg);
#else
#define IF_THROW(expr, msg)
#endif

#ifdef VERBOSE_LOGGING_ENABLED
/** Log an item with source location data. */
#define V_LOG(item); \
        nc::debug::LogToDiagnostics(item); \
        nc::debug::LogToDiagnostics(std::string("    Func: ") + NC_SOURCE_FUNCTION); \
        nc::debug::LogToDiagnostics(std::string("    File: ") + NC_SOURCE_FILE); \
        nc::debug::LogToDiagnostics(std::string("    Line: ") + std::to_string(NC_SOURCE_LINE));
#else
    #define V_LOG(item);
#endif
