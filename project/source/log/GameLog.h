#pragma once
#include <deque>
#include <string>

namespace project::log
{
    class GameLog
    {
        public:
            GameLog();
            ~GameLog();

            static void Log(std::string item) ;
            static const std::deque<std::string>& GetItems();

            void Log_(std::string item);

        private:
            static GameLog* m_instance;
            std::deque<std::string> m_items;
            unsigned m_itemCount = 5;
    };
}