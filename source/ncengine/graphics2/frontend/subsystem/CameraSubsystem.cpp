#include "CameraSubsystem.h"
#include "CameraRenderState.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/window/Window.h"

namespace
{
constexpr auto g_defaultProperties = nc::graphics::CameraProperties{};
const auto g_defaultView = DirectX::XMMatrixLookAtRH(
    DirectX::g_XMIdentityR3,
    DirectX::g_XMIdentityR2,
    DirectX::g_XMNegIdentityR1
);

auto MakeDefaultViewProjection() -> DirectX::XMMATRIX
{
    const auto [width, height] = nc::window::GetScreenExtent();
    return DirectX::XMMatrixMultiply(
        g_defaultView,
        DirectX::XMMatrixPerspectiveFovRH(
            g_defaultProperties.fov,
            width / height,
            g_defaultProperties.nearClip,
            g_defaultProperties.farClip
        )
    );
}
} // anonymous namespace

namespace nc::graphics
{
auto CameraSubsystem::BuildState(ecs::ExplicitEcs<Transform> ecs) -> CameraRenderState
{
    if (m_mainCamera)
    {
        const auto& transform = ecs.Get<Transform>(m_mainCamera->ParentEntity());
        m_mainCamera->UpdateViewMatrix(transform.TransformationMatrix());

        return CameraRenderState{
            .viewProjection = DirectX::XMMatrixMultiply(
                m_mainCamera->ViewMatrix(),
                m_mainCamera->ProjectionMatrix()
            ),
            .invProjection = m_mainCamera->InverseProjectionMatrix(),
            .position = transform.Position()
        };
    }

    auto viewProj = MakeDefaultViewProjection();
    auto inverseProj = DirectX::XMMatrixInverse(nullptr, viewProj);

    return CameraRenderState{
        .viewProjection = viewProj,
        .invProjection = inverseProj,
        .position = Vector3::Zero()
    };
}
} // namespace nc::graphics
