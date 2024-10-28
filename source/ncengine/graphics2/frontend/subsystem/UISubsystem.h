#pragma once

#include "ncengine/ui/IUI.h"
#include "ncengine/ui/editor/Editor.h"

namespace nc::graphics
{
class UISubsystem
{
    public:
        UISubsystem(ecs::Ecs world,
                    ModuleProvider modules,
                    SystemEvents& events)
            : m_editor{ui::editor::BuildEditor(world, modules, events)}
        {
        }

        auto IsHovered() const noexcept -> bool
        {
            return m_clientUI ? m_clientUI->IsHovered() : false;
        }

        void SetClientUI(ui::IUI* ui) noexcept
        {
            m_clientUI = ui;
        }

        void UpdateUI(ecs::Ecs world)
        {
            m_editor->Draw(world);
            if (m_clientUI)
            {
                m_clientUI->Draw();
            }
        }

    private:
        std::unique_ptr<ui::editor::Editor> m_editor;
        ui::IUI* m_clientUI = nullptr;
};
} // namespace nc::graphics
