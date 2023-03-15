#include "CameraSystem.h"
#include "ecs/Registry.h"
#include "graphics/Camera.h"

namespace nc::graphics
{
auto CameraSystem::Execute(Registry* registry) -> CameraState
{
    if (!m_mainCamera)
    {
        return CameraState{};
    }

    m_mainCamera->UpdateViewMatrix();
    const auto& transform = registry->Get<Transform>(m_mainCamera->ParentEntity());

    return CameraState
    {
        .view = m_mainCamera->ViewMatrix(),
        .projection = m_mainCamera->ProjectionMatrix(),
        .position = transform->Position(),
        .frustum = m_mainCamera->CalculateFrustum(),
        .hasCamera = true
    };
}
} // namespace nc::graphics
