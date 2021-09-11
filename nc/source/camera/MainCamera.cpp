#include "MainCamera.h"
#include "MainCameraInternal.h"
#include "Ecs.h"
#include "debug/Utils.h"

namespace
{
    nc::Entity g_mainCamera = nc::Entity::Null();
    DirectX::XMMATRIX g_viewMatrix;
    DirectX::XMMATRIX g_projectionMatrix;
}

namespace nc::camera
{
    /** Api Functions */
    void SetMainCamera(Camera* camera)
    {
        V_LOG("Setting main camera");
        g_mainCamera = camera->GetParentEntity();
    }

    Transform* GetMainCameraTransform()
    {
        IF_THROW(!g_mainCamera.Valid(), "camera::GetMainCameraTransform - No camera is set");
        return ActiveRegistry()->Get<Transform>(g_mainCamera);
    }

    /** Internal Functions */
    void ClearMainCamera()
    {
        g_mainCamera = Entity::Null();
    }

    void UpdateViewMatrix()
    {
        IF_THROW(!g_mainCamera.Valid(), "camera::UpdateViewMatrix - No camera is set");
        auto* transform = ActiveRegistry()->Get<Transform>(g_mainCamera);
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform->GetTransformationMatrix());
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        g_viewMatrix = DirectX::XMMatrixLookAtRH(pos_v, pos_v + look_v, DirectX::g_XMNegIdentityR1);
    }

    void UpdateProjectionMatrix(float width, float height, float nearZ, float farZ)
    {
        g_projectionMatrix = DirectX::XMMatrixPerspectiveRH(1.0f, height / width, nearZ, farZ);
    }

    DirectX::FXMMATRIX GetViewMatrix()
    {
        return g_viewMatrix;
    }

    DirectX::FXMMATRIX GetProjectionMatrix()
    {
        return g_projectionMatrix;
    }

    Frustum CalculateFrustum()
    {
        IF_THROW(!g_mainCamera.Valid(), "camera::CalculateFrustum - No camera is set");

        const auto* transform = ActiveRegistry()->Get<Transform>(g_mainCamera);
        const auto world = transform->GetTransformationMatrix();
        const auto m = DirectX::XMMatrixTranspose(g_viewMatrix * g_projectionMatrix);

        auto left_v = DirectX::XMPlaneNormalize(m.r[3] + m.r[0]);
        auto right_v = DirectX::XMPlaneNormalize(m.r[3] - m.r[0]);
        auto bottom_v = DirectX::XMPlaneNormalize(m.r[3] + m.r[1]);
        auto top_v = DirectX::XMPlaneNormalize(m.r[3] - m.r[1]);
        auto front_v = DirectX::XMPlaneNormalize(m.r[2]);
        auto back_v = DirectX::XMPlaneNormalize(m.r[3] - m.r[2]);

        Frustum out;
        DirectX::XMStoreVector3(&out.left.normal, left_v);
        out.left.d = -1.0f * DirectX::XMVectorGetW(left_v);
        DirectX::XMStoreVector3(&out.right.normal, right_v);
        out.right.d = -1.0f * DirectX::XMVectorGetW(right_v);
        DirectX::XMStoreVector3(&out.bottom.normal, bottom_v);
        out.bottom.d = -1.0f * DirectX::XMVectorGetW(bottom_v);
        DirectX::XMStoreVector3(&out.top.normal, top_v);
        out.top.d = -1.0f * DirectX::XMVectorGetW(top_v);
        DirectX::XMStoreVector3(&out.front.normal, front_v);
        out.front.d = -1.0f * DirectX::XMVectorGetW(front_v);
        DirectX::XMStoreVector3(&out.back.normal, back_v);
        out.back.d = -1.0f * DirectX::XMVectorGetW(back_v);

        return out;
    }
}
