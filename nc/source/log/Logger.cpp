#include "Logger.h"

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

namespace nc::log
{
    Logger::Logger(std::string path)
    {
        m_file.open(path);
        if(!m_file.is_open())
        {
            throw std::runtime_error("Log - failed to open file: " + path);
        }
        m_file << "Log started: " << GetTime();
    }

    Logger::~Logger()
    {
        m_file << "Log finished: " << GetTime();
        m_file.close();
    }

    void Logger::Log(std::string item)
    {
        m_file << item << '\n';
    }
}