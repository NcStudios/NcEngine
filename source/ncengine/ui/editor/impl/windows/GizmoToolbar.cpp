#include "GizmoToolbar.h"

#include "ncengine/Events.h"
#include "ncengine/ui/editor/EditorContext.h"
#include "ncengine/input/Input.h"
#include "ncengine/graphics/NcGraphics.h"

#include "ncengine/ui/ImGuiUtility.h"
#include "ncengine/ui/ImGuiStyle.h"
#include "imgui.h"
#include "ImGuizmo.h"

namespace
{
constexpr auto g_buttonSize = ImVec2{16.0f, 16.0f};
constexpr auto g_gridSize = 10.0f;
constexpr auto g_selectedColor = nc::ui::color::Green;
constexpr auto g_unselectedColor = nc::ui::default_scheme::Border;
constexpr auto g_identityMatrix = DirectX::XMFLOAT4X4{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

auto GetTranslateSnapValues(nc::ui::editor::TransformGizmoOptions& options) -> float*
{
    return &options.snapValues[nc::ui::editor::TransformGizmoOptions::TranslateSnapValueOffset];
}

auto GetRotateSnapValues(nc::ui::editor::TransformGizmoOptions& options) -> float*
{
    return &options.snapValues[nc::ui::editor::TransformGizmoOptions::RotateSnapValueOffset];
}

auto GetScaleSnapValues(nc::ui::editor::TransformGizmoOptions& options) -> float*
{
    return &options.snapValues[nc::ui::editor::TransformGizmoOptions::ScaleSnapValueOffset];
}

auto GetSnapValues(nc::ui::editor::TransformGizmoOptions& options) -> float*
{
    using namespace nc::ui::editor;
    if (!options.enableSnap)
    {
        return nullptr;
    }

    switch (options.mode)
    {
        case TransformGizmoMode::Translate: return GetTranslateSnapValues(options);
        case TransformGizmoMode::Rotate:    return GetRotateSnapValues(options);
        case TransformGizmoMode::Scale:     return GetScaleSnapValues(options);
        default:
            NC_ASSERT(false, "Unhandled TransformGizmoMode");
            std::unreachable();
    }
};

struct ViewProjection
{
    DirectX::XMFLOAT4X4 view = DirectX::XMFLOAT4X4{};
    DirectX::XMFLOAT4X4 proj = DirectX::XMFLOAT4X4{};
};

auto GetViewProjectionMatrices(nc::ui::editor::EditorContext& ctx) -> ViewProjection
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

auto IsTransformGizmoAllowed(const nc::ui::editor::EditorContext& ctx) -> bool
{
    return !ctx.selectedEntity.IsStatic() || ctx.rebuildStaticsOnTransformWrite;
}

void SetImGuizmoRect()
{
    const auto& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
}

void MoveObject(nc::ui::editor::EditorContext& ctx,
                nc::ui::editor::TransformGizmoMode mode,
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
            using enum nc::ui::editor::TransformGizmoMode;
            case Translate: body.SetSimulatedBodyPosition(transform, nc::ToVector3(pos),    false); break;
            case Rotate:    body.SetSimulatedBodyRotation(transform, nc::ToQuaternion(rot), false); break;
            case Scale:     body.SetSimulatedBodyScale(   transform, nc::ToVector3(scl),    false); break;
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
    // handle this...
    const auto info = asset::FontInfo{"icons.ttf", 12.0f};
    const auto font = asset::AcquireFont(info);
    IMGUI_SCOPE(StyleFont, font.font);
    IMGUI_SCOPE(StyleVar, ImGuiStyleVar_FramePadding, ImVec2{1.0f, 1.0f});
    IMGUI_SCOPE(StyleVar, ImGuiStyleVar_ItemSpacing, ImVec2{1.0f, 1.0f});


    // move these...
    const auto gridIcon = reinterpret_cast<const char*>(u8"\uea71");
    const auto translateIcon = reinterpret_cast<const char*>(u8"\uea3c");
    const auto rotateIcon = reinterpret_cast<const char*>(u8"\ue984");
    const auto scaleIcon = reinterpret_cast<const char*>(u8"\ue989");
    const auto lockedIcon = reinterpret_cast<const char*>(u8"\ue98f");
    const auto unlockedIcon = reinterpret_cast<const char*>(u8"\ue990");

    static auto toolbarButton = [](const char* label,
                            const char* tooltip,
                            bool isSelected,
                            auto&& onClick)
    {
        IMGUI_SCOPE(StyleColor, ImGuiCol_Border, isSelected ? g_selectedColor : g_unselectedColor);
        ImGui::SameLine();
        if (ImGui::Button(label, g_buttonSize))
        {
            onClick();
        }

        SetTooltip(tooltip);
    };

    static auto toggleButton = [](const char* label,
                           const char* tooltip,
                           bool& value)
    {
        toolbarButton(label, tooltip, value, [&value]() { value = !value; });
    };

    static auto selectionButton = [](const char* label,
                                     const char* tooltip,
                                     auto targetValue,
                                     auto& value)
    {
        toolbarButton(label, tooltip, targetValue == value, [&value, targetValue]() { value = targetValue; });
    };

    auto toolbarSpacing = []()
    {
        ImGui::SameLine();
        ImGui::Text(" ");
    };

    toggleButton(gridIcon, "Show Grid", m_gridGizmo.enabled);
    toolbarSpacing();

    {
        IMGUI_SCOPE(DisableIf, !ctx.selectedEntity.Valid());
        selectionButton(translateIcon, "Translate Mode", TransformGizmoMode::Translate, m_transformGizmo.mode);
        selectionButton(rotateIcon,    "Rotate Mode",    TransformGizmoMode::Rotate,    m_transformGizmo.mode);
        selectionButton(scaleIcon,     "Scale Mode",     TransformGizmoMode::Scale,     m_transformGizmo.mode);
        toolbarSpacing();
        const auto snapIcon = m_transformGizmo.enableSnap ? lockedIcon : unlockedIcon;
        toggleButton(snapIcon, "Enable Snap", m_transformGizmo.enableSnap);

        {
            IMGUI_SCOPE(ui::DisableIf, !m_transformGizmo.enableSnap);

            ImGui::SameLine();
            ImGui::SetNextItemWidth(150.0f);
            constexpr auto label = "##snap";
            constexpr auto fmt = "%.3f";
            constexpr auto flags = ImGuiSliderFlags_AlwaysClamp;
            switch (m_transformGizmo.mode)
            {
                case TransformGizmoMode::Translate:
                    ImGui::DragFloat3(label, GetTranslateSnapValues(m_transformGizmo), 1.0f, 0.001f, g_maxPos, fmt, flags);
                    break;
                case TransformGizmoMode::Rotate:
                    ImGui::DragFloat(label, GetRotateSnapValues(m_transformGizmo), 0.1f, 0.001f, g_maxAngle, fmt, flags);
                    break;
                case TransformGizmoMode::Scale:
                    ImGui::DragFloat(label, GetScaleSnapValues(m_transformGizmo), 0.5f, g_minScale, g_maxScale, fmt, flags);
                    break;
                default:
                    NC_ASSERT(false, "Unhandled TransformGizmoMode");
                    std::unreachable();
            }

            SetTooltip("Snap To");
        }
    }
}

void GizmoToolbar::DrawGizmos(EditorContext& ctx)
{
    ImGuizmo::Enable(IsTransformGizmoAllowed(ctx));
    SetImGuizmoRect();

    if (KeyHeld(input::KeyCode::LeftCtrl))
    {
        if (KeyDown(ctx.hotkeys.translateMode)) m_transformGizmo.mode = TransformGizmoMode::Translate;
        if (KeyDown(ctx.hotkeys.rotateMode))    m_transformGizmo.mode = TransformGizmoMode::Rotate;
        if (KeyDown(ctx.hotkeys.scaleMode))     m_transformGizmo.mode = TransformGizmoMode::Scale;
    }

    const auto [view, proj] = GetViewProjectionMatrices(ctx);

    if (m_gridGizmo.enabled)
    {
        ImGuizmo::DrawGrid(&view._11, &proj._11, &g_identityMatrix._11, g_gridSize);
    }

    if (ctx.selectedEntity.Valid())
    {
        auto& transform = ctx.world.Get<Transform>(ctx.selectedEntity);
        auto model = GetModelMatrix(transform);
        const auto op = static_cast<ImGuizmo::OPERATION>(m_transformGizmo.mode);
        const auto snap = GetSnapValues(m_transformGizmo);
        ImGuizmo::Manipulate(&view._11, &proj._11, op, ImGuizmo::LOCAL, &model._11, nullptr, snap);

        if (ImGuizmo::IsUsing())
        {
            MoveObject(ctx, m_transformGizmo.mode, transform, model);
        }
    }
}
} // namespace nc::ui::editor
