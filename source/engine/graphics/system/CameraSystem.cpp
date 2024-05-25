#include "CameraSystem.h"
#include "ecs/Registry.h"
#include "graphics/Camera.h"

namespace nc::graphics
{
auto CameraSystem::Execute(Registry* registry) -> CameraState
{
    if (!m_mainCamera)
    {
        return CameraState
        {
            .view = DirectX::XMMatrixIdentity(),
            .projection = DirectX::XMMatrixIdentity(),
            .position = Vector3::Zero(),
            .frustum = Frustum{}
        };
    }

    const auto transform = registry->Get<Transform>(m_mainCamera->ParentEntity());
    m_mainCamera->UpdateViewMatrix(transform->TransformationMatrix());

    return CameraState
    {
        .view = m_mainCamera->ViewMatrix(),
        .projection = m_mainCamera->ProjectionMatrix(),
        .position = transform->Position(),
        .frustum = m_mainCamera->CalculateFrustum()
    };
}
} // namespace nc::graphics
