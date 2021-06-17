#include "MainCamera.h"
#include "MainCameraInternal.h"
#include "Ecs.h"
#include "debug/Utils.h"

namespace
{
    nc::Entity g_mainCamera = nc::Entity::Null();
}

namespace nc::camera
{
    /* Api Function Implementation */
    void SetMainCamera(Camera* camera)
    {
        V_LOG("Setting main camera");
        g_mainCamera = camera->GetParentEntity();
    }

    void ClearMainCamera()
    {
        g_mainCamera = Entity::Null();
    }

    DirectX::XMMATRIX CalculateViewMatrix()
    {
        IF_THROW(!g_mainCamera.Valid(), "camera::CalculateViewMatrix - No camera is set");
        auto* transform = ActiveRegistry()->Get<Transform>(g_mainCamera);
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, transform->GetTransformationMatrix());
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtLH(pos_v, pos_v + look_v, DirectX::g_XMIdentityR1);
    }
    
    Transform* GetMainCameraTransform()
    {
        IF_THROW(!g_mainCamera.Valid(), "camera::GetMainCameraTransform - No camera is set");
        return ActiveRegistry()->Get<Transform>(g_mainCamera);
    }
}
