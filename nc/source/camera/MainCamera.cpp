#include "MainCamera.h"
#include "DebugUtils.h"

namespace nc::camera
{
    std::function<void(Camera*)> MainCamera::Set_ = nullptr;
    std::function<Transform*()> MainCamera::GetTransform_ = nullptr;

    void MainCamera::Set(Camera* camera)
    {
        V_LOG("Setting main camera");
        IF_THROW(!MainCamera::Set_, "MainCamera::Set_ is not bound");
        MainCamera::Set_(camera);
    }
    
    Transform* MainCamera::GetTransform()
    {
        IF_THROW(!MainCamera::GetTransform_, "MainCamera::GetTransform_ is not bound");
        return MainCamera::GetTransform_();
    }
}