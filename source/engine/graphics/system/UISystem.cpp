#include "UISystem.h"
#include "ui/IUI.h"
#include "ui/editor/Editor.h"

namespace nc::graphics
{
UISystem::UISystem(ecs::Ecs world, std::function<void(std::unique_ptr<Scene>)> changeScene)
    : m_editor{ui::editor::BuildEditor(world, std::move(changeScene))}
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
