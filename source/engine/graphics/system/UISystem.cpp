#include "UISystem.h"
#include "ncengine/ui/IUI.h"
#include "ncengine/ui/Editor.h"

namespace nc::graphics
{
UISystem::UISystem(std::unique_ptr<ui::editor::Editor> editor)
    : m_editor{std::move(editor)}
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

void UISystem::Execute(ecs::Ecs world, asset::NcAsset& assetModule)
{
    m_editor->Draw(world, assetModule);

    if (m_clientUI)
    {
        m_clientUI->Draw();
    }
}
} // nc::graphics
