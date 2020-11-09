#pragma once

#include <fstream>

namespace nc::log
{
    class Logger
    {
        public:
            Logger(std::string path);
            ~Logger();
            static void Log(std::string);
        
        private:
            void Log_(std::string);

            static Logger* m_instance;
            std::ofstream m_file;
    };
}