#pragma once

#include <memory>

namespace nc
{
class Registry;

namespace ui
{
class IUI;

namespace editor
{
class Editor;
} // namespace editor
} // namespace ui

namespace graphics
{
class UISystem
{
    public:
        UISystem();
        ~UISystem() noexcept;

        auto IsHovered() const noexcept -> bool;
        void SetClientUI(ui::IUI* ui) noexcept;
        void Execute(Registry* registry);

    private:
        std::unique_ptr<ui::editor::Editor> m_editor;
        ui::IUI* m_clientUI = nullptr;
};
} // namespace graphics
} // namespace nc
