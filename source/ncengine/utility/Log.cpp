#include "ncengine/utility/Log.h"
#include "ncengine/config/Config.h"

#include "ncutility/NcError.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>

namespace
{
/** @todo MSVC warns simply that we're using time_t. It'd be better to avoid ctime
 *        altogether - new chrono stuff may offer a better way? */
auto GetDateTime()
{
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4996 )
#endif
    const auto timePoint = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(timePoint);
    return std::ctime(&time);
#if defined(_MSC_VER)
#pragma warning( pop )
#endif
}

struct LogEntry
{
    std::string message;
    char prefix;
};

constexpr auto MaxMessageCount = 20ull;
auto g_mutex = std::mutex{};
auto g_messages = std::vector<LogEntry>{};
auto g_logPath = std::string{};
auto g_logMaxSize = 0ull;

void FlushLogToFile() noexcept
{
    try
    {
        if (std::filesystem::file_size(g_logPath) > g_logMaxSize)
        {
            std::filesystem::rename(g_logPath, g_logPath + ".1");
        }

        auto file = std::ofstream{g_logPath, std::ios::app};
        for(const auto& item : g_messages)
        {
            file << item.prefix << ' ' << item.message << '\n';
        }

        g_messages.clear();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
} // anonymous namespace

namespace nc::utility::detail
{
void InitializeLog(const config::ProjectSettings& settings)
{
    const auto lock = std::lock_guard{g_mutex};
    g_logPath = settings.logFilePath;
    g_logMaxSize = settings.logMaxFileSize;
    auto file = std::ofstream{g_logPath, std::ios::out | std::ios::app};
    if (!file)
    {
        throw NcError(fmt::format("Failed to initialize log: {}", g_logPath));
    }

    file << "Log started: " << ::GetDateTime();
}

void CloseLog() noexcept
{
    const auto lock = std::lock_guard{g_mutex};
    ::FlushLogToFile();
    /** @todo Reopening the file here is a wild choice, but attempts to queue up the
     *        time_t as a string before the flush segfault. Fix when ctime is gone. */
    auto file = std::ofstream{g_logPath, std::ios::app};
    file << "Log ended: " << ::GetDateTime() << '\n';
}

void Log(char prefix, std::string_view item) noexcept
{
    const auto lock = std::lock_guard{g_mutex};
    g_messages.emplace_back(std::string{item}, prefix);
    if (g_messages.size() > MaxMessageCount)
    {
        ::FlushLogToFile();
    }
}

void Log(char prefix, std::string_view file, int line, std::string_view item) noexcept
{
    Log(prefix, fmt::format("{}:{}: {}", file, line, item));
}

void Log(const std::exception& e) noexcept
{
    Log('E', fmt::format("***Exception*** {}", e.what()));
    std::cerr << e.what();

    try
    {
        std::rethrow_if_nested(e);
    }
    catch(const std::exception& rethrown)
    {
        Log(rethrown);
    }
}
} // namespace nc::utility
