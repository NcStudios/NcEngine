#include "ecs/component/Camera.h"
#include "config/Config.h"
#include "ecs/Registry.h"
#include "Window.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    Camera::Camera(Entity entity) noexcept
        : FreeComponent(entity),
          m_view{},
          m_projection{}
    {
        auto [width, height] = window::GetDimensions();
        const auto& graphicsSettings = config::GetGraphicsSettings();
        UpdateProjectionMatrix(width, height, graphicsSettings.nearClip, graphicsSettings.farClip);
    }

    void Camera::UpdateViewMatrix()
    {
        const auto* transform = ActiveRegistry()->Get<Transform>(ParentEntity());
        const auto& m = transform->TransformationMatrix();
        const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, m);
        m_view = DirectX::XMMatrixLookAtRH(m.r[3], look, DirectX::g_XMNegIdentityR1);
    }

    void Camera::UpdateProjectionMatrix(float width, float height, float nearZ, float farZ)
    {
        m_projection = DirectX::XMMatrixPerspectiveRH(1.0f, height / width, nearZ, farZ);
    }

    auto Camera::CalculateFrustum() const noexcept -> Frustum
    {
        using namespace DirectX;
        Frustum out;
        const auto m = XMMatrixTranspose(m_view * m_projection);

        const auto left_v = XMPlaneNormalize(XMVectorAdd(m.r[3], m.r[0]));
        XMStoreVector3(&out.left.normal, left_v);
        out.left.d = -1.0f * XMVectorGetW(left_v);

        const auto right_v = XMPlaneNormalize(XMVectorSubtract(m.r[3], m.r[0]));
        XMStoreVector3(&out.right.normal, right_v);
        out.right.d = -1.0f * XMVectorGetW(right_v);

        const auto bottom_v = XMPlaneNormalize(XMVectorAdd(m.r[3], m.r[1]));
        XMStoreVector3(&out.bottom.normal, bottom_v);
        out.bottom.d = -1.0f * XMVectorGetW(bottom_v);

        const auto top_v = XMPlaneNormalize(XMVectorSubtract(m.r[3], m.r[1]));
        XMStoreVector3(&out.top.normal, top_v);
        out.top.d = -1.0f * XMVectorGetW(top_v);

        const auto front_v = XMPlaneNormalize(m.r[2]);
        XMStoreVector3(&out.front.normal, front_v);
        out.front.d = -1.0f * XMVectorGetW(front_v);

        const auto back_v = XMPlaneNormalize(XMVectorSubtract(m.r[3], m.r[2]));
        XMStoreVector3(&out.back.normal, back_v);
        out.back.d = -1.0f * XMVectorGetW(back_v);

        return out;
    }

    #ifdef NC_EDITOR_ENABLED
    void Camera::ComponentGuiElement()
    {
        ImGui::Text("Camera");
    }
    #endif
}