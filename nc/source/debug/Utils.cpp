#include "debug/Utils.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>

namespace nc::debug
{
    namespace internal
    {
        std::ofstream file;

        auto GetTime()
        {
            auto timePoint = std::chrono::system_clock::now();
            std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
            return std::ctime(&time);
        }

        void OpenLog(std::string_view path)
        {
            internal::file.open(path.data());
            if(!internal::file.is_open())
            {
                throw NcError(std::string{"Failed to open log file: "} + path.data());
            }
            internal::file << "Log started: " << GetTime();
        }

        void CloseLog() noexcept
        {
            if(internal::file.is_open())
            {
                internal::file << "Log finished: " << GetTime();
                internal::file.close();
            }
        }
    } // end namespace internal

    void LogToDiagnostics(const std::string& item) noexcept
    {
        if(internal::file.is_open())
        {
            internal::file << item << '\n';
            return;
        }
    }

    void LogException(const std::exception& e) noexcept
    {
        std::cerr << e.what();

        LogToDiagnostics("***Exception***");
        LogToDiagnostics(e.what());

        try
        {
            std::rethrow_if_nested(e);
        }
        catch(const std::exception& e)
        {
            LogException(e);
        }
    }
}