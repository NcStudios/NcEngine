#include "utility/Log.h"
#include "ncutility/NcError.h"

#include <chrono>
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

void FlushLogToFile() noexcept
{
    auto file = std::ofstream{g_logPath, std::ios::app};
    for(const auto& item : g_messages)
    {
        file << item.prefix << ' ' << item.message << '\n';
    }

    g_messages.clear();
}
} // anonymous namespace

namespace nc::utility::detail
{
void InitializeLog(std::string_view path)
{
    const auto lock = std::lock_guard{g_mutex};
    g_logPath = path;
    auto file = std::ofstream{g_logPath, std::ios::out | std::ios::trunc};
    if (!file)
    {
        throw NcError(fmt::format("Failed to initialize log: {}", path));
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
    file << "Log ended: " << ::GetDateTime();
}

void Log(std::string_view item, char prefix) noexcept
{
    const auto lock = std::lock_guard{g_mutex};
    g_messages.emplace_back(std::string{item}, prefix);
    if (g_messages.size() > MaxMessageCount)
    {
        ::FlushLogToFile();
    }
}

void Log(const std::exception& e) noexcept
{
    Log(fmt::format("***Exception*** {}", e.what()), 'E');
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
