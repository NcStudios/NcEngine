#pragma once

#include <functional>
#include <stdexcept>
#include <string>

namespace nc::engine { class ApiBinder; }

namespace nc::debug
{
    class Log
    {
        public:
            static void LogToDiagnostics(std::string item);

        private:
            friend class nc::engine::ApiBinder;
            static std::function<void(std::string)> LogToDiagnostics_;
    };
}

#ifdef NC_DEBUG_BUILD
    namespace nc::debug
    {
        inline void IfThrow_(bool expr, const char* msg)
        {
            if(expr)
                throw std::runtime_error(msg);
        }
    }
    #define IF_THROW(expr, msg) nc::debug::IfThrow_(expr, msg)
#else
    #define IF_THROW(expr, msg)
#endif

#ifdef VERBOSE_LOGGING_ENABLED
#define V_LOG(item); \
        nc::debug::Log::LogToDiagnostics(item); \
        nc::debug::Log::LogToDiagnostics(std::string("    Func: ") + __PRETTY_FUNCTION__); \
        nc::debug::Log::LogToDiagnostics(std::string("    File: ") + __FILE__); \
        nc::debug::Log::LogToDiagnostics(std::string("    Line: ") + std::to_string(__LINE__));
#else
    #define V_LOG(item);
#endif
