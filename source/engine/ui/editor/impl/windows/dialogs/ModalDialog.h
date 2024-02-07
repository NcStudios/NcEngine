#pragma once

#include "ncengine/ui/ImGuiUtility.h"

namespace nc::ui::editor
{
class ModalDialog
{
    public:
        auto IsOpen() const noexcept -> bool
        {
            return m_isOpen;
        }

    protected:
        explicit ModalDialog(Vector2 size) noexcept
            : m_size{size.x, size.y} {}

        auto OpenPopup()
        {
            m_isOpen = true;
        }

        void ClosePopup()
        {
            m_isOpen = false;
            ImGui::CloseCurrentPopup();
        }

        void DrawPopup(const char* label, const ImVec2& dimensions, auto&& func)
        {
            ImGui::OpenPopup(label);
            ImGui::SetNextWindowSize(m_size, ImGuiCond_Once);
            ImGui::SetNextWindowPos(dimensions * 0.5f, ImGuiCond_Once, ImVec2{0.5f, 0.5f});
            if (ImGui::BeginPopupModal(label, &m_isOpen))
            {
                func();
                ImGui::EndPopup();
            }
        }

    private:
        ImVec2 m_size;
        bool m_isOpen = false;
};
} // namespace nc::ui::editor
