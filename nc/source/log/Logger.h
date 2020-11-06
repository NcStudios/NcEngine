#pragma once

#include <fstream>

namespace nc::log
{
    class Logger
    {
        public:
            Logger(std::string path);
            ~Logger();
            void Log(std::string);
        
        private:
            std::ofstream m_file;
    };
}