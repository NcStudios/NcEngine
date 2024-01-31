#pragma once

#include "ncengine/ecs/Ecs.h"
#include "ncengine/module/ModuleProvider.h"

#include <memory>

namespace nc
{
namespace asset
{
class NcAsset;
} // namespace asset

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
        UISystem(ecs::Ecs world, ModuleProvider modules);
        ~UISystem() noexcept;

        auto IsHovered() const noexcept -> bool;
        void SetClientUI(ui::IUI* ui) noexcept;
        void Execute(ecs::Ecs world);

    private:
        std::unique_ptr<ui::editor::Editor> m_editor;
        ui::IUI* m_clientUI = nullptr;
};
} // namespace graphics
} // namespace nc
