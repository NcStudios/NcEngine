#include "LightSubsystem.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Light.h"

#include <ranges>

namespace pointlight2
{
constexpr float g_lightFieldOfView = nc::DegreesToRadians(60.0f);
constexpr float g_nearClip = 0.25f;
constexpr float g_farClip = 96.0f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(DirectX::FXMMATRIX transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // namespace pointlight2

namespace spotlight2
{
    constexpr float g_nearClip = 0.1f;
    constexpr float g_defaultFarClip = 100.0f;

    auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix, float outerAngle, float radius, DirectX::XMVECTOR lightDirection) -> DirectX::XMMATRIX
    {
        // Compute the original cosine of the outer angle
        float cosOuterAngle = cos(outerAngle);

        // Apply the radius modulation with clamping to prevent excessive widening
        float modulationFactor = std::clamp(1.0f - radius, 0.001f, 1.5f);
        float modulatedCos = cosOuterAngle * modulationFactor;

        // Ensure the modulated cosine is in a valid range [-1, 1] to avoid NaN from acos
        modulatedCos = std::clamp(modulatedCos, -1.0f, 1.0f);

        // Compute the effective outer angle in radians
        float effectiveOuterAngle = acos(modulatedCos);

        // The FOV for the shadow map should be 2 * effectiveOuterAngle to match the cone
        float lightFieldOfView = 3.14159f * effectiveOuterAngle;

        // Use the light's radius as the far clip plane (with a small margin)
        float farClip = radius * 1.1f;

        // Create a perspective projection matrix that matches the cone
        auto lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(lightFieldOfView, 1.0f, g_nearClip, farClip);

        // Extract the light's position from the transform matrix
        DirectX::XMVECTOR lightPosition = transformMatrix.r[3];

        // Compute the look-at point: position + direction
        DirectX::XMVECTOR lookAtPoint = DirectX::XMVectorAdd(lightPosition, lightDirection);

        // Create the view matrix using the light's position and direction
        return DirectX::XMMatrixLookAtRH(lightPosition, lookAtPoint, DirectX::g_XMIdentityR1) * lightProjectionMatrix;
    }
}
namespace nc::graphics
{
auto LightSubsystem::BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs) -> LightRenderState
{
    m_data.clear();

    { // Directional Lights
        const auto& pool = ecs.GetPool<DirectionalLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(light.diffuseColor, light.specularColor, light.intensity, transform.Forward());
        }
    }

    { // Point Lights
        const auto& pool = ecs.GetPool<PointLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                0, /** @todo, come up with shadow decisioning (which lights cast shadows) */
                light.radius,
                pointlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix())
            );
        }
    }

    { // Spot Lights
                // Spot Lights
                const auto& pool = ecs.GetPool<SpotLight>();
                for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
                {
                    float rawOuterAngle = std::max<float>(light.outerAngle, 0.0001f); // In radians
                    float radius = std::max<float>(light.radius, 0.0001f);
                
                    float innerAngle = cos(std::max<float>(light.innerAngle, 0.0001f));
                    float outerAngle = cos(rawOuterAngle);
                
                    auto& transform = ecs.Get<Transform>(entity);
                    DirectX::XMVECTOR lightDirection = transform.ForwardXM(); // Get the forward direction
                
                    m_data.emplace_back(
                        light.diffuseColor,
                        light.specularColor,
                        light.intensity,
                        transform.Position(),
                        innerAngle,
                        transform.Forward(), // Pass the direction directly
                        outerAngle,
                        radius,
                        light.castsShadows,
                        spotlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix(), rawOuterAngle, radius, lightDirection)
                    );
                }
    }
    return LightRenderState{m_data};
}
} // namespace nc::graphics
