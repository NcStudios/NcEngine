#include "UISystem.h"
#include "ui/IUI.h"
#include "ui/editor/Editor.h"

namespace nc::graphics
{
UISystem::UISystem()
    : m_editor{ui::editor::BuildEditor()}
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
