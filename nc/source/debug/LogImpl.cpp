#include "LogImpl.h"

#include <chrono>
#include <ctime>

namespace
{
    auto GetTime()
    {
        auto timePoint = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
        return std::ctime(&time);
    }
}

namespace nc::debug
{
    LogImpl::LogImpl(std::string path)
    {
        m_file.open(path);
        if(!m_file.is_open())
        {
            throw std::runtime_error("Log - failed to open file: " + path);
        }
        m_file << "Log started: " << GetTime();
    }

    LogImpl::~LogImpl()
    {
        m_file << "Log finished: " << GetTime();
        m_file.close();
    }

    void LogImpl::LogToDiagnostics(std::string item)
    {
        m_file << item << '\n';
    }
}