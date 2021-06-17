#include "GameLog.h"

namespace nc::sample
{
    GameLog* GameLog::m_instance = nullptr;

    GameLog::GameLog()
    {
        GameLog::m_instance = this;
    }

    GameLog::~GameLog() noexcept
    {
        GameLog::m_instance = nullptr;
    }

    void GameLog::SetItemCount(unsigned count)
    {
        m_itemCount = count;
        while(m_items.size() > m_itemCount)
            m_items.pop_front();
    }
    
    void GameLog::Clear()
    {
        m_items.clear();
    }

    // double fuck bun
    void GameLog::ClearStatic()
    {
        GameLog::m_instance->Clear();
    }

    const std::deque<std::string>& GameLog::GetItems()
    {
        return m_items;
    }

    void GameLog::Log(std::string item)
    {
        auto& items = GameLog::m_instance->m_items;
        auto itemCount = GameLog::m_instance->m_itemCount;
        items.push_back(std::move(item));
        if(items.size() > itemCount)
            items.pop_front();
    }
}