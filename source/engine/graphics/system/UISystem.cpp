#include "UISystem.h"
#include "ncengine/ui/IUI.h"
#include "ncengine/ui/Editor.h"

namespace nc::graphics
{
UISystem::UISystem(ecs::Ecs world, ModuleProvider modules, SystemEvents& events)
    : m_editor{ui::editor::BuildEditor(world, modules, events)}
{
}

UISystem::~UISystem() noexcept = default;

auto UISystem::IsHovered() const noexcept -> bool
{
    return m_clientUI ? m_clientUI->IsHovered() : false;
}

void UISystem::SetClientUI(ui::IUI* ui) noexcept
{
    m_clientUI = ui;
}

void UISystem::Execute(ecs::Ecs world)
{
    m_editor->Draw(world);

    if (m_clientUI)
    {
        m_clientUI->Draw();
    }
}
} // nc::graphics
