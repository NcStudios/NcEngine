#include "UISubsystem.h"

#include "ncengine/ui/IUI.h"
#include "ncengine/ui/editor/Editor.h"

namespace nc::graphics
{
UISubsystem::UISubsystem(ecs::Ecs world,
                         ModuleProvider modules,
                         SystemEvents& events)
    : m_editor{ui::editor::BuildEditor(world, modules, events)}
{
}

UISubsystem::~UISubsystem() noexcept = default;

auto UISubsystem::IsHovered() const noexcept -> bool
{
    return m_clientUI ? m_clientUI->IsHovered() : false;
}

void UISubsystem::SetClientUI(ui::IUI* ui) noexcept
{
    m_clientUI = ui;
}

void UISubsystem::UpdateUI(ecs::Ecs world)
{
    m_editor->Draw(world);
    if (m_clientUI)
    {
        m_clientUI->Draw();
    }
}
} // namespace nc::graphics
