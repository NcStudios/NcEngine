#include "CameraSubsystem.h"
#include "CameraRenderState.h"

#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/window/Window.h"

namespace
{
auto MakeDefaultViewProjection() -> DirectX::XMMATRIX
{
    return DirectX::XMMatrixMultiply(
        nc::MakeDefaultViewMatrix(),
        nc::MakeDefaultProjectionMatrix()
    );
}
} // anonymous namespace

namespace nc::graphics
{
auto CameraSubsystem::BuildState(ecs::ExplicitEcs<Transform> ecs) -> CameraRenderState
{
    NC_PROFILE_SCOPE("CameraSubsystem::BuildState", ProfileCategory::Rendering);
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

    auto viewProj = MakeDefaultViewProjection();
    auto inverseProj = DirectX::XMMatrixInverse(nullptr, viewProj);

    return CameraRenderState{
        .viewProjection = viewProj,
        .invProjection = inverseProj,
        .position = Vector3::Zero(),
        .nearClip = CameraProperties::DefaultNearClip,
        .farClip = CameraProperties::DefaultFarClip
    };
}
} // namespace nc::graphics
