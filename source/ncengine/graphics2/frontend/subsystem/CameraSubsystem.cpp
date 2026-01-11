#include "CameraSubsystem.h"
#include "CameraRenderState.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/window/Window.h"

namespace nc::graphics
{
auto CameraSubsystem::BuildState(ecs::ExplicitEcs<Transform> ecs) -> CameraRenderState
{
    if (m_mainCamera)
    {
        const auto& transform = ecs.Get<Transform>(m_mainCamera->ParentEntity());
        m_mainCamera->UpdateViewMatrix(transform.TransformationMatrix());
        const auto& properties = m_mainCamera->GetProperties();

        return CameraRenderState{
            .viewProjection = DirectX::XMMatrixMultiply(
                m_mainCamera->ViewMatrix(),
                m_mainCamera->ProjectionMatrix()
            ),
            .invProjection = m_mainCamera->InverseProjectionMatrix(),
            .position = transform.Position(),
            .nearClip = properties.nearClip,
            .farClip = properties.farClip
        };
    }

    auto view = MakeDefaultViewMatrix();
    auto proj = MakeDefaultProjectionMatrix();
    auto viewProj = DirectX::XMMatrixMultiply(view, proj);
    auto inverseProj = DirectX::XMMatrixInverse(nullptr, proj);

    return CameraRenderState{
        .viewProjection = viewProj,
        .invProjection = inverseProj,
        .position = Vector3::Zero(),
        .nearClip = CameraProperties::DefaultNearClip,
        .farClip = CameraProperties::DefaultFarClip
    };
}
} // namespace nc::graphics
