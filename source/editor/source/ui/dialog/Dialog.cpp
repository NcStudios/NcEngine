#include "Dialog.h"

#include "ncengine/window/Window.h"

namespace
{
    ImVec2 ToImVec2(nc::Vector2 vec)
    {
        return ImVec2{vec.x, vec.y};
    }

    ImVec2 FindCenter(nc::Vector2 vec)
    {
        return ToImVec2(vec * 0.5f);
    }
}

namespace nc::editor
{
    DialogBase::DialogBase(std::string title, Vector2 size)
        : m_title{std::move(title)},
          m_position{FindCenter(window::GetDimensions())},
          m_size{ToImVec2(size)},
          m_isOpen{false}
    {
        window::RegisterOnResizeReceiver(this);
    }
    
    DialogBase::~DialogBase()
    {
        window::UnregisterOnResizeReceiver(this);
    }

    DialogFlexible::DialogFlexible(std::string title, Vector2 size)
        : DialogBase(std::move(title), size)
    {
    }

    bool DialogFlexible::BeginWindow()
    {
        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Once, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize(), ImGuiCond_Once);
        return ImGui::Begin(m_title.c_str(), &m_isOpen);
    }

    void DialogFlexible::EndWindow()
    {
        ImGui::End();
    }

    void DialogFlexible::OnResize(Vector2)
    {
    }

    DialogFixedCentered::DialogFixedCentered(std::string title, Vector2 size)
        : DialogBase(std::move(title), size)
    {
    }

    void DialogFixedCentered::OnResize(Vector2 dimensions)
    {
        m_position = FindCenter(dimensions);
    }

    bool DialogFixedCentered::BeginWindow()
    {
        ImGui::SetNextWindowPos(GetPosition(), ImGuiCond_Always, ImVec2{0.5f, 0.5f});
        ImGui::SetNextWindowSize(GetSize());
        return ImGui::Begin(m_title.c_str(), &m_isOpen);
    }

    void DialogFixedCentered::EndWindow()
    {
        ImGui::End();
    }
}