#include "MainCameraImpl.h"

namespace nc::camera
{
    void MainCameraImpl::Set(Camera* camera) noexcept
    {
        m_camera = camera;
    }
    
    auto MainCameraImpl::Get() noexcept -> Camera*
    {
        return m_camera;
    }
}