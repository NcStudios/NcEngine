#include "GizmoToolbar.h"

#include "ncengine/Events.h"
#include "ncengine/ui/editor/EditorContext.h"
#include "ncengine/input/Input.h"
#include "ncengine/graphics/NcGraphics.h"

#include "ncengine/ui/ImGuiStyle.h"
#include "ncengine/ui/ImGuiUtility.h"
#include "ImGuizmo.h"

namespace
{
using namespace nc::ui::editor;

constexpr auto g_buttonSize = ImVec2{16.0f, 16.0f};
constexpr auto g_selectedColor = nc::ui::color::Green;
constexpr auto g_unselectedColor = nc::ui::default_scheme::Border;

auto GetTranslateSnapValues(GizmoOptions& options) -> float*
{
    return &options.snapValues[GizmoOptions::TranslateSnapValueOffset];
}

auto GetRotateSnapValues(GizmoOptions& options) -> float*
{
    return &options.snapValues[GizmoOptions::RotateSnapValueOffset];
}

auto GetScaleSnapValues(GizmoOptions& options) -> float*
{
    return &options.snapValues[GizmoOptions::ScaleSnapValueOffset];
}

auto GetSnapValues(GizmoOptions& options) -> float*
{
    if (!options.enableSnap)
    {
        return nullptr;
    }

    switch (options.mode)
    {
        case GizmoMode::Translate: return GetTranslateSnapValues(options);
        case GizmoMode::Rotate:    return GetRotateSnapValues(options);
        case GizmoMode::Scale:     return GetScaleSnapValues(options);
        default:
            NC_ASSERT(false, "Unhandled GizmoMode");
            std::unreachable();
    }
};

struct ViewProjection
{
    DirectX::XMFLOAT4X4 view = DirectX::XMFLOAT4X4{};
    DirectX::XMFLOAT4X4 proj = DirectX::XMFLOAT4X4{};
};

auto GetViewProjectionMatrices(EditorContext& ctx) -> ViewProjection
{
    const auto gfx = ctx.modules.Get<nc::NcGraphics>();
    const auto cam = gfx->GetCamera();
    auto out = ViewProjection{};

    if (cam)
    {
        DirectX::XMStoreFloat4x4(&out.view, cam->ViewMatrix());
        DirectX::XMStoreFloat4x4(&out.proj, cam->ProjectionMatrix());
    }
    else
    {
        DirectX::XMStoreFloat4x4(&out.view, nc::MakeDefaultViewMatrix());
        DirectX::XMStoreFloat4x4(&out.proj, nc::MakeDefaultProjectionMatrix());
    }

    return out;
}

auto GetModelMatrix(const nc::Transform& transform) -> DirectX::XMFLOAT4X4
{
    auto out = DirectX::XMFLOAT4X4{};
    DirectX::XMStoreFloat4x4(&out, transform.TransformationMatrix());
    return out;
}

auto IsTransformGizmoAllowed(const EditorContext& ctx) -> bool
{
    return !ctx.selectedEntity.IsStatic() || ctx.rebuildStaticsOnTransformWrite;
}

void SetImGuizmoRect()
{
    const auto& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
}

void PollHotkeys(const EditorHotkeys& hotkeys, GizmoOptions& options)
{
    if (KeyHeld(nc::input::KeyCode::LeftCtrl))
    {
        if (KeyDown(hotkeys.translateMode)) options.mode = GizmoMode::Translate;
        if (KeyDown(hotkeys.rotateMode))    options.mode = GizmoMode::Rotate;
        if (KeyDown(hotkeys.scaleMode))     options.mode = GizmoMode::Scale;
    }
}

void ToolbarButton(const char* label,
                   const char* tooltip,
                   auto targetValue,
                   auto& value)
{
    const auto& color = targetValue == value ? g_selectedColor : g_unselectedColor;
    IMGUI_SCOPE(nc::ui::StyleColor, ImGuiCol_Border, color);
    ImGui::SameLine();
    if (ImGui::Button(label, g_buttonSize))
    {
        value = targetValue;
    }

    nc::ui::SetTooltip(tooltip);
};

void MoveObject(EditorContext& ctx,
                GizmoMode mode,
                nc::Transform& transform,
                const DirectX::XMFLOAT4X4& model)
{
    auto newModel = DirectX::XMLoadFloat4x4(&model);

    if (ctx.world.Contains<nc::RigidBody>(ctx.selectedEntity))
    {
        auto& body = ctx.world.Get<nc::RigidBody>(ctx.selectedEntity);
        const auto [scl, rot, pos] = nc::DecomposeMatrix(newModel);
        switch (mode)
        {
            case GizmoMode::Translate: body.SetSimulatedBodyPosition(transform, nc::ToVector3(pos),    false); break;
            case GizmoMode::Rotate:    body.SetSimulatedBodyRotation(transform, nc::ToQuaternion(rot), false); break;
            case GizmoMode::Scale:     body.SetSimulatedBodyScale(   transform, nc::ToVector3(scl),    false); break;
            default:
                NC_ASSERT(false, "Unhandled ImGuizmo mode");
                std::unreachable();
        }
    }
    else
    {
        transform.SetTransformationMatrix(newModel);
    }

    if (ctx.selectedEntity.IsStatic() && ctx.rebuildStaticsOnTransformWrite)
    {
        ctx.events->rebuildStatics();
    }
}
} // anonymous namespace

namespace nc::ui::editor
{
void GizmoToolbar::DrawToolbar(EditorContext& ctx)
{
    IMGUI_SCOPE(StyleVar, ImGuiStyleVar_FramePadding, ImVec2{1.0f, 1.0f});
    IMGUI_SCOPE(StyleVar, ImGuiStyleVar_ItemSpacing, ImVec2{1.0f, 1.0f});
    PollHotkeys(ctx.hotkeys, m_options);

    {
        IMGUI_SCOPE(DisableIf, !ctx.selectedEntity.Valid());
        ToolbarButton("T", "Translate Mode", GizmoMode::Translate, m_options.mode);
        ToolbarButton("R",    "Rotate Mode", GizmoMode::Rotate,    m_options.mode);
        ToolbarButton("S",     "Scale Mode", GizmoMode::Scale,     m_options.mode);

        {
            IMGUI_SCOPE(ui::DisableIf, !m_options.enableSnap);
            SameLineSpaced();
            SameLineSpaced();
            ImGui::SetNextItemWidth(150.0f);
            constexpr auto label = "##snapto";
            constexpr auto fmt = "%.3f";
            constexpr auto flags = ImGuiSliderFlags_AlwaysClamp;
            switch (m_options.mode)
            {
                case GizmoMode::Translate:
                    ImGui::DragFloat3(label, GetTranslateSnapValues(m_options), 1.0f, 0.001f, g_maxPos, fmt, flags);
                    break;
                case GizmoMode::Rotate:
                    ImGui::DragFloat(label, GetRotateSnapValues(m_options), 0.1f, 0.001f, g_maxAngle, fmt, flags);
                    break;
                case GizmoMode::Scale:
                    ImGui::DragFloat(label, GetScaleSnapValues(m_options), 0.5f, g_minScale, g_maxScale, fmt, flags);
                    break;
                default:
                    NC_ASSERT(false, "Unhandled GizmoMode");
                    std::unreachable();
            }

            SetTooltip("Snap To");
        }

        SameLineSpaced();
        SameLineSpaced();
        ui::Checkbox(m_options.enableSnap, "Snap");
        SetTooltip("Enable Snap");
    }
}

void GizmoToolbar::DrawGizmos(EditorContext& ctx)
{
    ImGuizmo::Enable(IsTransformGizmoAllowed(ctx));
    SetImGuizmoRect();
    const auto [view, proj] = GetViewProjectionMatrices(ctx);

    if (ctx.selectedEntity.Valid())
    {
        auto& transform = ctx.world.Get<Transform>(ctx.selectedEntity);
        auto model = GetModelMatrix(transform);
        const auto op = static_cast<ImGuizmo::OPERATION>(m_options.mode);
        const auto snap = GetSnapValues(m_options);
        ImGuizmo::Manipulate(&view._11, &proj._11, op, ImGuizmo::LOCAL, &model._11, nullptr, snap);

        if (ImGuizmo::IsUsing())
        {
            MoveObject(ctx, m_options.mode, transform, model);
        }
    }
}
} // namespace nc::ui::editor
