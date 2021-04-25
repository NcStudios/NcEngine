#include "MainCamera.h"
#include "MainCameraInternal.h"
#include "Ecs.h"

namespace
{
    nc::Transform* g_mainCameraTransform = nullptr;
}

namespace nc::camera
{
    /* Api Function Implementation */
    void SetMainCamera(Camera* camera)
    {
        V_LOG("Setting main camera");
        g_mainCameraTransform = GetComponent<Transform>(camera->GetParentHandle());
    }

    void ClearMainCamera()
    {
        g_mainCameraTransform = nullptr;
    }

    DirectX::XMMATRIX CalculateViewMatrix()
    {
        IF_THROW(!g_mainCameraTransform, "camera::CalculateViewMatrix - No camera is set");
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, g_mainCameraTransform->GetTransformationMatrix());
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtLH(pos_v, pos_v + look_v, DirectX::g_XMIdentityR1);
    }
    
    Transform* GetMainCameraTransform()
    {
        IF_THROW(!g_mainCameraTransform, "camera::GetMainCameraTransform - No camera is set");
        return g_mainCameraTransform;
    }
}
