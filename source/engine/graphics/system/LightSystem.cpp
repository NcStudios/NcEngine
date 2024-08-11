#include "LightSystem.h"

#include "optick.h"

namespace pointlight
{
constexpr float g_lightFieldOfView = DirectX::XM_PIDIV2; // 90 degrees
constexpr float g_nearClip = 0.1f;
constexpr float g_farClip = 1024.0f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveFovRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

// auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& viewMatrix) -> DirectX::XMMATRIX
// {
//     return viewMatrix * g_lightProjectionMatrix;
// }

// auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix) -> DirectX::XMMATRIX
// {
//     const auto look = DirectX::XMVector3TransformNormal(DirectX::g_XMIdentityR2, transformMatrix); // Transform normal for direction
//     const auto eyePosition = transformMatrix.r[3];          // Extract translation component
//     return DirectX::XMMatrixLookAtRH(eyePosition,
//                                      DirectX::XMVectorAdd(eyePosition, look),
//                                      DirectX::g_XMIdentityR1) *  g_lightProjectionMatrix;
// }

// auto GetLightViewMatrix(const DirectX::XMVECTOR& lookAtDir, DirectX::XMVECTOR& upDir, const DirectX::XMVECTOR& lightPos, bool ) -> DirectX::XMMATRIX
// {
//     // if (negateZ)
//     //     return DirectX::XMMatrixLookAtRH(lightPos, lookAtDir, DirectX::XMVectorNegate(upDir));
//     return DirectX::XMMatrixLookAtRH(lightPos, lookAtDir, upDir);
// }

} // namespace pointlight

namespace spotlight
{
constexpr float g_lightFieldOfView = nc::DegreesToRadians(45.0f);
constexpr float g_nearClip = 1.0f;
constexpr float g_farClip = 100.f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // namespace spotlight

namespace nc::graphics
{
LightSystem::LightSystem(ShaderResourceBus* shaderResourceBus, uint32_t maxPointLights, uint32_t maxSpotLights, bool useShadows)
    : m_pointLightBuffer{shaderResourceBus->CreateStorageBuffer(sizeof(PointLightData) * maxPointLights, ShaderStage::Fragment | ShaderStage::Vertex, 1, 0, false)},
      m_spotLightBuffer{shaderResourceBus->CreateStorageBuffer(sizeof(SpotLightData) * maxSpotLights, ShaderStage::Fragment | ShaderStage::Vertex, 8, 0, false)},
      m_useShadows{useShadows}
{
    m_pointLightData.reserve(maxPointLights);
    m_spotLightData.reserve(maxSpotLights);
}

auto LightSystem::Execute(uint32_t currentFrameIndex, MultiView<PointLight, Transform> pointLights, MultiView<SpotLight, Transform> spotLights) -> LightState
{
    OPTICK_CATEGORY("LightSystem::Execute", Optick::Category::Rendering);
    auto state = LightState{};
    
    // Sync point lights
    m_pointLightData.clear();
    auto pointLightsCount = 0u;

    for (const auto& [light, transform] : pointLights)
    {
        pointLightsCount++;
        if (m_useShadows)
        {
            // Define the perspective projection matrix for a 90-degree field of view
            // viewMtxPosZ *= DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), -DirectX::XM_PI);

            // Positive X
            {
                const auto look = DirectX::XMVector3Transform(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), transform->TransformationMatrix());
                auto viewMatrix = DirectX::XMMatrixLookAtRH(transform->PositionXM(), look, DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
                state.viewProjections.push_back(viewMatrix * pointlight::g_lightProjectionMatrix); // Combine the view and projection matrices
            }
            // Negative X
            {
                const auto look = DirectX::XMVector3Transform(DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f), transform->TransformationMatrix());
                auto viewMatrix = DirectX::XMMatrixLookAtRH(transform->PositionXM(), look, DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
                state.viewProjections.push_back(viewMatrix * pointlight::g_lightProjectionMatrix); // Combine the view and projection matrices
            }
            // Positive Y
            {
                const auto look = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), transform->TransformationMatrix());
                auto viewMatrix = DirectX::XMMatrixLookAtRH(transform->PositionXM(), look, DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
                state.viewProjections.push_back(viewMatrix * pointlight::g_lightProjectionMatrix); // Combine the view and projection matrices
            }
            // Negative Y
            {
                const auto look = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f), transform->TransformationMatrix());
                auto viewMatrix = DirectX::XMMatrixLookAtRH(transform->PositionXM(), look, DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
                state.viewProjections.push_back(viewMatrix * pointlight::g_lightProjectionMatrix); // Combine the view and projection matrices
            }
            // Positive Z
            {
                const auto look = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), transform->TransformationMatrix());
                auto viewMatrix = DirectX::XMMatrixLookAtRH(transform->PositionXM(), look, DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
                state.viewProjections.push_back(viewMatrix * pointlight::g_lightProjectionMatrix); // Combine the view and projection matrices
            }
            // Negative Z
            {
                const auto look = DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), transform->TransformationMatrix());
                auto viewMatrix = DirectX::XMMatrixLookAtRH(transform->PositionXM(), look, DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
                state.viewProjections.push_back(viewMatrix * pointlight::g_lightProjectionMatrix); // Combine the view and projection matrices
            }

        }
        m_pointLightData.emplace_back(state.viewProjections.back(),
                                      transform->Position(),
                                      m_useShadows,
                                      light->ambientColor,
                                      light->diffuseColor,
                                      light->radius);
    }
    state.omniDirectionalLightCount += pointLightsCount;

    m_pointLightBuffer.Bind(m_pointLightData, currentFrameIndex);

    // Sync spot lights
    m_spotLightData.clear();
    auto spotLightsCount = 0u;

    for (const auto& [light, transform] : spotLights)
    {
        spotLightsCount++;
        state.viewProjections.push_back(spotlight::CalculateLightViewProjectionMatrix(transform->TransformationMatrix()));
        m_spotLightData.emplace_back(state.viewProjections.back(),
                                     transform->Position(),
                                     m_useShadows,
                                     light->color,
                                     transform->Forward(),
                                     std::cos(light->innerAngle),
                                     std::cos(light->outerAngle),
                                     light->radius);
    }
    state.uniDirectionalLightCount += spotLightsCount;

    m_spotLightBuffer.Bind(m_spotLightData, currentFrameIndex);
    if (m_useShadows && m_syncedLightsCount.at(currentFrameIndex) != pointLightsCount + spotLightsCount)
    {
        state.updateShadows = true;
    }
    m_syncedLightsCount.at(currentFrameIndex) = pointLightsCount + spotLightsCount;
    return state;
}

void LightSystem::Clear() noexcept
{
    m_pointLightData.clear();
    m_pointLightBuffer.Clear();
    m_spotLightData.clear();
    m_spotLightBuffer.Clear();
    for (auto& lightCount : m_syncedLightsCount)
    {
        lightCount = 0u;
    }
}
} // namespace nc::graphics
