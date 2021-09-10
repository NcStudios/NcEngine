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
}
