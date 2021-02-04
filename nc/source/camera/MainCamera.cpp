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
    
    Transform* GetMainCameraTransform()
    {
        IF_THROW(!g_mainCameraTransform, "camera::GetMainCameraTransform - No camera is set");
        return g_mainCameraTransform;
    }
}
