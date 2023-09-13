#include "UISystem.h"
#include "ui/IUI.h"
#include "ui/editor/Editor.h"

namespace
{
auto BuildEditor() -> std::unique_ptr<nc::ui::editor::Editor>
{
#ifdef NC_EDITOR_ENABLED
    return std::make_unique<nc::ui::editor::Editor>();
#else
    return nullptr;
#endif
}
} // anonymous namespace

namespace nc::graphics
{
UISystem::UISystem()
    : m_editor{::BuildEditor()}
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

void UISystem::Execute(Registry* registry)
{
    if (m_editor)
    {
        float dtFactor = 1.0f;
        m_editor->Frame(&dtFactor, registry);
    }

    if (m_clientUI)
    {
        m_clientUI->Draw();
    }
}
} // nc::graphics
