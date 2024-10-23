#include "CameraSubsystem.h"
#include "CameraRenderState.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Camera.h"

namespace
{
const auto g_defaultViewProj = DirectX::XMMatrixMultiply(
    DirectX::XMMatrixLookAtRH(
        DirectX::g_XMIdentityR3,
        DirectX::g_XMIdentityR2,
        DirectX::g_XMNegIdentityR1
    ),
    DirectX::XMMatrixPerspectiveFovRH(
        1.0472f,
        16.0f / 9.0f,
        0.1f,
        400.0f
    )
);
} // anonymous namespace

namespace nc::graphics
{
auto CameraSubsystem::BuildState(ecs::ExplicitEcs<Transform> ecs) -> CameraRenderState
{
    if (!m_mainCamera)
    {
        return CameraRenderState{
            .viewProjection = g_defaultViewProj
        };
    }

    const auto& transform = ecs.Get<Transform>(m_mainCamera->ParentEntity());
    m_mainCamera->UpdateViewMatrix(transform.TransformationMatrix());

    return CameraRenderState{
        .viewProjection = DirectX::XMMatrixMultiply(
            m_mainCamera->ViewMatrix(),
            m_mainCamera->ProjectionMatrix()
        )
    };
}
} // namespace nc::graphics
