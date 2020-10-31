#pragma once

#include <deque>

#include "log/ILog.h"

namespace project
{
    class GameLog : public nc::log::ILog
    {
        public:
            void AddItem(std::string item) override;
            const std::deque<std::string>& GetItems() const;

        private:
            std::deque<std::string> m_items;
            unsigned m_itemCount = 5;
    };
}