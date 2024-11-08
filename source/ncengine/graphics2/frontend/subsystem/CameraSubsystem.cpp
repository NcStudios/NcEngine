#include "CameraSubsystem.h"
#include "CameraRenderState.h"
#include "graphics2/frontend/GraphicsUtilities.h"

#include "ncengine/config/Config.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/graphics/Camera.h"
#include "ncengine/window/Window.h"

namespace
{
constexpr auto g_defaultProperties = nc::graphics::CameraProperties{};
const auto g_defaultViewRH = DirectX::XMMatrixLookAtRH(
    DirectX::g_XMIdentityR3,
    DirectX::g_XMIdentityR2,
    DirectX::g_XMNegIdentityR1
);

const auto g_defaultViewLH = DirectX::XMMatrixLookAtLH(
    DirectX::g_XMIdentityR3,
    DirectX::g_XMIdentityR2,
    DirectX::g_XMNegIdentityR1
);

auto MakeDefaultViewProjectionRH() -> DirectX::XMMATRIX
{
    const auto [width, height] = nc::window::GetScreenExtent();
    return DirectX::XMMatrixMultiply(
        g_defaultViewRH,
        DirectX::XMMatrixPerspectiveFovRH(
            g_defaultProperties.fov,
            width / height,
            g_defaultProperties.nearClip,
            g_defaultProperties.farClip
        )
    );
}

auto MakeDefaultViewProjectionLH() -> DirectX::XMMATRIX
{
    const auto [width, height] = nc::window::GetScreenExtent();
    return DirectX::XMMatrixMultiply(
        g_defaultViewLH,
        DirectX::XMMatrixPerspectiveFovLH(
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
CameraSubsystem::CameraSubsystem(const config::GraphicsSettings& graphicsSettings)
    : m_isRightHanded{graphicsSettings.api == api::Vulkan}
{
    if (m_mainCamera)
    {
        auto [width, height] = window::GetScreenExtent();
        m_mainCamera->UpdateProjectionMatrix(width, height, m_isRightHanded);
    }
}

auto CameraSubsystem::BuildState(ecs::ExplicitEcs<Transform> ecs) -> CameraRenderState
{
    if (m_mainCamera)
    {
        const auto& transform = ecs.Get<Transform>(m_mainCamera->ParentEntity());
        m_mainCamera->UpdateViewMatrix(transform.TransformationMatrix(), m_isRightHanded);

        return CameraRenderState{
            .viewProjection = DirectX::XMMatrixMultiply(
                m_mainCamera->ViewMatrix(),
                m_mainCamera->ProjectionMatrix()
            )
        };
    }

    return CameraRenderState{
        .viewProjection = m_isRightHanded ? MakeDefaultViewProjectionRH() : MakeDefaultViewProjectionLH()
    };
}
} // namespace nc::graphics
