#include "GameLog.h"

namespace nc::sample
{
    GameLog* GameLog::m_instance = nullptr;

    GameLog::GameLog()
    {
        GameLog::m_instance = this;
    }

    GameLog::~GameLog()
    {
        GameLog::m_instance = nullptr;
    }

    void GameLog::Log(std::string item)
    {
        auto& items = GameLog::m_instance->m_items;
        auto itemCount = GameLog::m_instance->m_itemCount;
        items.push_back(std::move(item));
        if(items.size() > itemCount)
            items.pop_front();
    }

    const std::deque<std::string>& GameLog::GetItems()
    {
        return GameLog::m_instance->m_items;
    }
}