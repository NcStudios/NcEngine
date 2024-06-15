#pragma once

#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/ui/editor/EditorContext.h"

#include <memory>

namespace nc::ui::editor
{
// Flags to keep editor objects out of the way
constexpr auto EditorObjectFlags = nc::Entity::Flags::Persistent |
                                   nc::Entity::Flags::Internal |
                                   nc::Entity::Flags::NoSerialize;

class Editor
{
    public:
        explicit Editor(const EditorContext& ctx) noexcept
            : m_ctx{ctx} {}

        virtual ~Editor() = default;
        virtual void Draw(ecs::Ecs world) = 0;

        auto GetContext() const noexcept -> const EditorContext&
        {
            return m_ctx;
        }

    protected:
        EditorContext m_ctx;
};

auto BuildEditor(ecs::Ecs world,
                 ModuleProvider modules,
                 SystemEvents& events,
                 const EditorHotkeys& hotkeys = EditorHotkeys{}) -> std::unique_ptr<Editor>;
} // namespace nc::ui::editor
