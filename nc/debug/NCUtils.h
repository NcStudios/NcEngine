#pragma once
#include <iostream>
//#include <fstream>
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