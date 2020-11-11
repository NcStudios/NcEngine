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
    Logger* Logger::m_instance = nullptr;

    Logger::Logger(std::string path)
    {
        Logger::m_instance = this;

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
        Logger::m_instance = nullptr;
    }

    void Logger::Log(std::string item)
    {
        Logger::m_instance->Log_(std::move(item));
    }

    void Logger::Log_(std::string item)
    {
        m_file << item << '\n';
    }
}