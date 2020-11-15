#include "MainCamera.h"
#include "DebugUtils.h"

namespace nc::camera
{
    std::function<void(Camera*)> MainCamera::Set_ = nullptr;
    std::function<Transform*()> MainCamera::GetTransform_ = nullptr;

    void MainCamera::Set(Camera* camera)
    {
        V_LOG("Setting main camera");
        MainCamera::Set_(camera);
    }
    
    Transform* MainCamera::GetTransform()
    {
        return MainCamera::GetTransform_();
    }
}