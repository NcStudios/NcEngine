#pragma once

#include "ncengine/ui/IUI.h"
#include "ncengine/ui/editor/Editor.h"

namespace nc
{
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
class UISubsystem
{
    public:
        UISubsystem(ecs::Ecs world,
                    ModuleProvider modules,
                    SystemEvents& events);

        ~UISubsystem() noexcept;

        auto IsHovered() const noexcept -> bool;
        void SetClientUI(ui::IUI* ui) noexcept;
        void UpdateUI(ecs::Ecs world);

    private:
        std::unique_ptr<ui::editor::Editor> m_editor;
        ui::IUI* m_clientUI = nullptr;
};
} // namespace graphics
} // namespace nc
