#pragma once

#include <fstream>

namespace nc::debug
{
    class LogImpl
    {
        public:
            LogImpl(std::string path);
            ~LogImpl();
            
            void LogToDiagnostics(std::string);
        
        private:
            std::ofstream m_file;
    };
}