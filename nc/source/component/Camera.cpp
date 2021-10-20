#include "component/Camera.h"
#include "config/Config.h"
#include "Ecs.h"
#include "Window.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    Camera::Camera(Entity entity) noexcept
        : AutoComponent(entity),
          m_view{},
          m_projection{}
    {
        auto [width, height] = window::GetDimensions();
        const auto& graphicsSettings = config::GetGraphicsSettings();
        UpdateProjectionMatrix(width, height, graphicsSettings.nearClip, graphicsSettings.farClip);
    }

    void Camera::UpdateViewMatrix()
    {
        const auto* transform = ActiveRegistry()->Get<Transform>(GetParentEntity());
        const auto& m = transform->GetTransformationMatrix();
        const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, m);
        m_view = DirectX::XMMatrixLookAtRH(m.r[3], look, DirectX::g_XMNegIdentityR1);

        // DirectX::XMVECTOR scl_v, rot_v, pos_v;
        // DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform->GetTransformationMatrix());
        // //auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        // auto look_v = DirectX::XMVector3Rotate(DirectX::g_XMIdentityR2, rot_v);
        // m_view = DirectX::XMMatrixLookAtRH(pos_v, pos_v + look_v, DirectX::g_XMNegIdentityR1);
    }

    void Camera::UpdateProjectionMatrix(float width, float height, float nearZ, float farZ)
    {
        m_projection = DirectX::XMMatrixPerspectiveRH(1.0f, height / width, nearZ, farZ);
    }

    auto Camera::CalculateFrustum() const noexcept -> Frustum
    {
        Frustum out;
        const auto m = DirectX::XMMatrixTranspose(m_view * m_projection);

        const auto left_v = DirectX::XMPlaneNormalize(m.r[3] + m.r[0]);
        DirectX::XMStoreVector3(&out.left.normal, left_v);
        out.left.d = -1.0f * DirectX::XMVectorGetW(left_v);
        
        const auto right_v = DirectX::XMPlaneNormalize(m.r[3] - m.r[0]);
        DirectX::XMStoreVector3(&out.right.normal, right_v);
        out.right.d = -1.0f * DirectX::XMVectorGetW(right_v);
        
        const auto bottom_v = DirectX::XMPlaneNormalize(m.r[3] + m.r[1]);
        DirectX::XMStoreVector3(&out.bottom.normal, bottom_v);
        out.bottom.d = -1.0f * DirectX::XMVectorGetW(bottom_v);
        
        const auto top_v = DirectX::XMPlaneNormalize(m.r[3] - m.r[1]);
        DirectX::XMStoreVector3(&out.top.normal, top_v);
        out.top.d = -1.0f * DirectX::XMVectorGetW(top_v);
        
        const auto front_v = DirectX::XMPlaneNormalize(m.r[2]);
        DirectX::XMStoreVector3(&out.front.normal, front_v);
        out.front.d = -1.0f * DirectX::XMVectorGetW(front_v);
        
        const auto back_v = DirectX::XMPlaneNormalize(m.r[3] - m.r[2]);
        DirectX::XMStoreVector3(&out.back.normal, back_v);
        out.back.d = -1.0f * DirectX::XMVectorGetW(back_v);

        return out;
    }

    #ifdef NC_EDITOR_ENABLED
    void Camera::ComponentGuiElement()
    {
        ImGui::Text("Camera");
    }
    #endif
}