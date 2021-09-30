#pragma once

#include <deque>
#include <string>

namespace nc::editor
{
    /** Editor log */
    class Output
    {
        public:
            static const unsigned DefaultItemCount = 50u;

            Output();
            ~Output() noexcept;
            void SetItemCount(unsigned count);
            void Clear();
            const std::deque<std::string>& GetItems();
            static void Log(std::string item);

            void Draw();

        private:
            static Output* m_instance;
            std::deque<std::string> m_items;
            unsigned m_itemCount = DefaultItemCount;
    };
}