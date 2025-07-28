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
    DirectX::XMStoreFloat4x4(&out, transform.LocalTransformationMatrix());
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

void PollHotkeys(const EditorHotkeys& hotkeys, GizmoMode& mode)
{
    if (KeyHeld(nc::input::KeyCode::LeftCtrl))
    {
        if (KeyDown(hotkeys.translateMode)) mode = GizmoMode::Translate;
        if (KeyDown(hotkeys.rotateMode))    mode = GizmoMode::Rotate;
        if (KeyDown(hotkeys.scaleMode))     mode = GizmoMode::Scale;
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
    PollHotkeys(ctx.hotkeys, m_mode);

    {
        IMGUI_SCOPE(DisableIf, !ctx.selectedEntity.Valid());
        ToolbarButton("T", "Translate Mode", GizmoMode::Translate, m_mode);
        ToolbarButton("R",    "Rotate Mode", GizmoMode::Rotate,    m_mode);
        ToolbarButton("S",     "Scale Mode", GizmoMode::Scale,     m_mode);
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
        const auto op = static_cast<ImGuizmo::OPERATION>(m_mode);
        ImGuizmo::Manipulate(&view._11, &proj._11, op, ImGuizmo::LOCAL, &model._11);

        if (ImGuizmo::IsUsing())
        {
            MoveObject(ctx, m_mode, transform, model);
        }
    }
}
} // namespace nc::ui::editor
