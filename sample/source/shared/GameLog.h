#pragma once

#include <deque>
#include <string>

namespace nc::sample
{
class GameLog
{
    public:
        static const unsigned DefaultItemCount = 10u; // SampleUI needs to initialize from same value

        GameLog();
        ~GameLog() noexcept;
        void SetItemCount(unsigned count);
        void Clear();
        const std::deque<std::string>& GetItems();
        static void Log(std::string item);

    private:
        static GameLog* m_instance;
        std::deque<std::string> m_items;
        unsigned m_itemCount = DefaultItemCount;
};
}
