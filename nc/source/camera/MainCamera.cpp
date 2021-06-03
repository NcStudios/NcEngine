#include "MainCamera.h"
#include "MainCameraInternal.h"
#include "Ecs.h"

namespace
{
    nc::EntityHandle g_mainCamera = nc::EntityHandle::Null();
}

namespace nc::camera
{
    /* Api Function Implementation */
    void SetMainCamera(Camera* camera)
    {
        V_LOG("Setting main camera");
        g_mainCamera = camera->GetParentHandle();
    }

    void ClearMainCamera()
    {
        g_mainCamera = EntityHandle::Null();
    }

    DirectX::XMMATRIX CalculateViewMatrix()
    {
        IF_THROW(!g_mainCamera.Valid(), "camera::CalculateViewMatrix - No camera is set");
        auto* transform = GetComponent<Transform>(g_mainCamera);
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform->GetTransformationMatrix());
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtLH(pos_v, pos_v + look_v, DirectX::g_XMIdentityR1);
    }
    
    Transform* GetMainCameraTransform()
    {
        IF_THROW(!g_mainCamera.Valid(), "camera::GetMainCameraTransform - No camera is set");
        return GetComponent<Transform>(g_mainCamera);
    }
}
