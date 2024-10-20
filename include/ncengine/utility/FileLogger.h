#pragma once

#include "Log.h"

#include "ncengine/type/StableAddress.h"

#include <mutex>
#include <string>
#include <vector>

namespace nc
{
/** @brief Basic file logging implementation. Construct this on the stack in main() to reroute logging to a file. */
class FileLogger : public StableAddress
{
    public:
        static constexpr auto DefaultMaxLogSize = 1000000ull;
        static constexpr auto DefaultMessageFlushCount = 20ull;

        FileLogger(std::string_view logFilePath,
                   size_t logMaxFileSize = DefaultMaxLogSize,
                   size_t messageFlushCount = DefaultMessageFlushCount);

        ~FileLogger() noexcept;

    private:
        static inline FileLogger* s_instance = nullptr;
        std::mutex m_mutex;
        std::vector<std::string> m_messages;
        std::string m_logPath;
        size_t m_maxSize;
        size_t m_messageFlushCount;

        static void Log(LogCategory category,
                        std::string_view subsystem,
                        std::string_view file,
                        int line,
                        std::string_view message);

        void BufferMessage(std::string&& message);
        void Flush() noexcept;
};
} // namespace nc
