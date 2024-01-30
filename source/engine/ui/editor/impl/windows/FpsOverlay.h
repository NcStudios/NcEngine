#pragma once

#include "ncengine/ui/ImGuiUtility.h"

namespace nc::ui::editor
{
class FpsOverlay
{
    static constexpr auto WindowFlags = ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoResize   |
                                        ImGuiWindowFlags_NoBackground;

    public:
        auto ToggleOpen() noexcept
        {
            m_open = !m_open;
        }

        auto IsOpen() const noexcept -> bool
        {
            return m_open;
        }

        void Draw(const ImVec2& dimensions)
        {
            ImGui::SetNextWindowPos({dimensions.x - 90, 0}, ImGuiCond_Once);
            ImGui::SetNextWindowSize({90, 40});
            if (ImGui::Begin("FpsOverlay", nullptr, WindowFlags))
            {
                ImGui::Text("fps: %.1f", ImGui::GetIO().Framerate);
            }

            ImGui::End();
        }

    private:
        bool m_open = false;
};
} // namespace nc::ui::editor
