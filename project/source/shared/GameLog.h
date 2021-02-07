#pragma once

#include <deque>
#include <string>

namespace nc::sample
{
    class GameLog
    {
        public:
            GameLog();
            ~GameLog();
            static void Log(std::string item) ;
            static const std::deque<std::string>& GetItems();

        private:
            static GameLog* m_instance;
            std::deque<std::string> m_items;
            unsigned m_itemCount = 10u;
    };
}