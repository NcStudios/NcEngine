#include "MainCamera.h"
#include "graphics/Camera.h"

namespace nc::camera
{
void MainCamera::Set(graphics::Camera* camera) noexcept
{
    m_camera = camera;
}

auto MainCamera::Get() noexcept -> graphics::Camera*
{
    return m_camera;
}
} // namespace nc::camera