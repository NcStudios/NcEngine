#pragma once

#include <deque>

#include "log/IGameLog.h"

namespace project::log
{
    class GameLog : public nc::log::IGameLog
    {
        public:
            void Log(std::string item) override;
            const std::deque<std::string>& GetItems() const;

        private:
            std::deque<std::string> m_items;
            unsigned m_itemCount = 5;
    };
}