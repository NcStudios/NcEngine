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

struct GizmoOptions
{
    static constexpr auto TranslateSnapValueOffset = 0u;
    static constexpr auto RotateSnapValueOffset = 3u;
    static constexpr auto ScaleSnapValueOffset = 4u;

    GizmoMode mode = GizmoMode::Translate;
    float snapValues[5] = {1.0f, 1.0f, 1.0f, 0.174533f, 1.0f};
    bool enableSnap = false;
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
        GizmoOptions m_options;
        bool m_open = true;
};
} // namespace nc::ui::editor
