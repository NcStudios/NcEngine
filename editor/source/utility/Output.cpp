#include "Output.h"

#include "imgui/imgui.h"

namespace nc::editor
{
    Output* Output::m_instance = nullptr;

    Output::Output()
    {
        Output::m_instance = this;
    }

    Output::~Output() noexcept
    {
        Output::m_instance = nullptr;
    }

    void Output::SetItemCount(unsigned count)
    {
        m_itemCount = count;
        while(m_items.size() > m_itemCount)
            m_items.pop_front();
    }
    
    void Output::Clear()
    {
        m_items.clear();
    }

    const std::deque<std::string>& Output::GetItems()
    {
        return m_items;
    }

    void Output::Log(std::string item)
    {
        auto& items = Output::m_instance->m_items;
        auto itemCount = Output::m_instance->m_itemCount;
        items.push_back(std::move(item));
        if(items.size() > itemCount)
            items.pop_front();
    }

    void Output::Draw()
    {
        for(const auto& item : m_items)
        {
            ImGui::Text(item.c_str());
            ImGui::Separator();
        }
    }
}