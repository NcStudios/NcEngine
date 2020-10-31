#include "GameLog.h"

namespace project
{
    void GameLog::AddItem(std::string item)
    {
        m_items.push_back(std::move(item));
        if(m_items.size() > m_itemCount)
        {
            m_items.pop_front();
        }
    }

    const std::deque<std::string>& GameLog::GetItems() const
    {
        return m_items;
    }
}