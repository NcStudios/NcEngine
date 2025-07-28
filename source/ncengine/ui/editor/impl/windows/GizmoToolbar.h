#pragma once

namespace nc::ui::editor
{
struct EditorContext;

/** @internal Keep values in sync w/ ImGuizmo::OPERATION */
enum class GizmoMode
{
    Translate = 7,
    Rotate    = 120,
    Scale     = 896
};

class GizmoToolbar
{
    public:
        auto ToggleOpen() noexcept
        {
            m_open = !m_open;
        }

        auto IsOpen() const noexcept -> bool
        {
            return m_open;
        }

        void DrawToolbar(EditorContext& ctx);
        void DrawGizmos(EditorContext& ctx);

    private:
        GizmoMode m_mode = GizmoMode::Translate;
        bool m_open = true;
};
} // namespace nc::ui::editor
