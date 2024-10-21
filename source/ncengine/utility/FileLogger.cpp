#include "ncengine/utility/FileLogger.h"

#include "ncutility/platform/Platform.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace
{
NC_DISABLE_WARNING_PUSH
NC_DISABLE_WARNING_MSVC(4996)
auto GetDateTime()
{
    const auto timePoint = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(timePoint);
    return std::ctime(&time);
}
NC_DISABLE_WARNING_POP
} // anonymous namespace

namespace nc
{
FileLogger::FileLogger(std::string_view logFilePath,
                       size_t logMaxFileSize,
                       size_t messageFlushCount)
    : m_logPath{logFilePath},
      m_maxSize{logMaxFileSize},
      m_messageFlushCount{messageFlushCount}
{
    if (!std::filesystem::exists(m_logPath))
    {
        std::ofstream{m_logPath};
    }

    s_instance = this;
    SetLogCallback(FileLogger::Log);
    NC_LOG_INFO("Log started: {}", ::GetDateTime());
}

FileLogger::~FileLogger() noexcept
{
    NC_LOG_INFO("Log ended: {}", ::GetDateTime());
    Flush();
    s_instance = nullptr;

    // Don't leave a dangling ptr behind, but verify cb hasn't already been changed.
    if (detail::LogCallback == FileLogger::Log)
    {
        SetLogCallback(detail::DefaultLogCallback);
    }
}

void FileLogger::Log(LogCategory category,
                     std::string_view subsystem,
                     std::string_view file,
                     int line,
                     std::string_view message)
{
    s_instance->BufferMessage(NC_LOG_FMT_MSG(category, subsystem, file, line, message));
}

void FileLogger::BufferMessage(std::string&& message)
{
    const auto lock = std::lock_guard{m_mutex};
    m_messages.push_back(std::move(message));
    if (m_messages.size() > m_messageFlushCount)
    {
        Flush();
    }
}

void FileLogger::Flush() noexcept
{
    try
    {
        if (std::filesystem::file_size(m_logPath) > m_maxSize)
        {
            std::filesystem::rename(m_logPath, m_logPath + ".1");
        }

        auto file = std::ofstream{m_logPath, std::ios::app};
        for(const auto& message : m_messages)
        {
            file << message;
        }

        m_messages.clear();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
} // namespace nc
