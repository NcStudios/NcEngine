#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/module/ModuleProvider.h"

namespace nc
{
struct SystemEvents;

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
