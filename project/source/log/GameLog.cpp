#include "GameLog.h"

namespace project::log
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
        GameLog::m_instance->Log_(std::move(item));
    }

    void GameLog::Log_(std::string item)
    {
        m_items.push_back(std::move(item));
        if(m_items.size() > m_itemCount)
        {
            m_items.pop_front();
        }
    }

    const std::deque<std::string>& GameLog::GetItems()
    {
        return GameLog::m_instance->m_items;
    }
}