#include "MainCameraImpl.h"

namespace nc::camera
{
    void MainCameraImpl::Set(Camera* camera)
    {
        m_camera = camera;
    }
    
    auto MainCameraImpl::Get() -> Camera*
    {
        return m_camera;
    }
}