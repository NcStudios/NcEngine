#ifndef NC_UTILS_H
#define NC_UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

namespace debug
{
    template<typename T>
    void Log(const T& val)
    {
        std::cout << val << std::endl;
    }

    template<typename T, typename ...Args>
    void Log(const T& val, Args... args)
    {
        std::cout << val;
        Log(args...);
    }

    // void LogDump(const std::string& msg)
    // {
    //     const std::string FILE_NAME = "LOG_DUMP.txt";
    //     std::ofstream outFile;
    //     outFile.open(FILE_NAME);
    //     std::time_t t = std::time(0);
    //     std::tm* now = std::localtime(&t);

    //     outFile << now->tm_hour     << ':' << now->tm_min  << ':' << now->tm_sec           << ' '
    //             << (now->tm_mon +1) << '/' << now->tm_mday << '/' << (now->tm_year + 1900) << '\n'
    //             << msg << '\n';
    // }

    template<typename T>
    void CountItems(const T& container, std::string containerName = "unnamed")
    {
        int count = 0;
        for(auto item : container)
        {
            ++count;
        }
        Log(containerName, " item count: ", count);
    }




}

#endif