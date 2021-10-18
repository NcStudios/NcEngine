#pragma once

#include <deque>
#include <string>

namespace nc::editor
{
    /** Editor log */
    class Output
    {
        struct LogItem
        {
            std::string text;
            std::string description;
            bool isError;
        };

        public:
            static const unsigned DefaultItemCount = 50u;

            Output();
            ~Output() noexcept;
            void SetItemCount(unsigned count);
            void Clear();
            static void Log(std::string item, std::string description = "", bool isError = false);
            static void LogError(std::string item, std::string description = "");

            void Draw();

        private:
            static Output* m_instance;
            std::deque<LogItem> m_items;
            unsigned m_itemCount = DefaultItemCount;
            bool m_needScroll = false;
    };
}