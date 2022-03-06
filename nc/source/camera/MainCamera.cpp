#include "MainCamera.h"
#include "ecs/component/Camera.h"

namespace nc::camera
{
    void MainCamera::Set(Camera* camera) noexcept
    {
        m_camera = camera;
    }
    
    auto MainCamera::Get() noexcept -> Camera*
    {
        return m_camera;
    }
}