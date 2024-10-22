#include "CameraSubsystem.h"
#include "CameraRenderState.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Camera.h"

namespace nc::graphics
{
auto CameraSubsystem::BuildState(ecs::ExplicitEcs<Transform> ecs) -> CameraRenderState
{
    if (!m_mainCamera)
    {
        return CameraRenderState
        {
            .viewProjection = DirectX::XMMatrixIdentity()
        };
    }

    const auto& transform = ecs.Get<Transform>(m_mainCamera->ParentEntity());
    m_mainCamera->UpdateViewMatrix(transform.TransformationMatrix());

    return CameraRenderState
    {
        .viewProjection = DirectX::XMMatrixMultiply(
            m_mainCamera->ViewMatrix(),
            m_mainCamera->ProjectionMatrix()
        )
    };
}
} // namespace nc::graphics
