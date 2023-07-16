#pragma once

#include "ncengine/ui/IUI.h"
#include "ncengine/window/IOnResizeReceiver.h"

#include <memory>

namespace nc::editor
{
class EditorUI : public ui::UIFlexible, public window::IOnResizeReceiver
{
    public:
        EditorUI();
        ~EditorUI() noexcept;

        void Draw() override;
        auto IsHovered() -> bool override { return false; }
        void OnResize(Vector2 dimensions) override;

    private:
        Vector2 m_dimensions;
};
} // namespace nc::editor
