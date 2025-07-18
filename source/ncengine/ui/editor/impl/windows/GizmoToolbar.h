#pragma once

namespace nc::ui::editor
{
struct EditorContext;

/** @note Keep values in sync w/ ImGuizmo::OPERATION */
enum class TransformGizmoMode
{
    Translate = 7,
    Rotate    = 120,
    Scale     = 896
};

struct TransformGizmoOptions
{
    static constexpr auto TranslateSnapValueOffset = 0u;
    static constexpr auto RotateSnapValueOffset = 3u;
    static constexpr auto ScaleSnapValueOffset = 4u;

    TransformGizmoMode mode = TransformGizmoMode::Translate;
    float snapValues[5] = {1.0f, 1.0f, 1.0f, 0.174533f, 1.0f};
    bool enableSnap = false;
};

struct GridGizmoOptions
{
    bool enabled = false;
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
        TransformGizmoMode m_mode = TransformGizmoMode::Translate;
        TransformGizmoOptions m_transformGizmo;
        GridGizmoOptions m_gridGizmo;
        bool m_open = true;
};

void GizmosWindow(EditorContext& ctx);
} // namespace nc::ui::editor
