#include "SpotLightSystem.h"

#include "optick.h"

namespace
{
constexpr float g_lightFieldOfView = nc::DegreesToRadians(60.0f);
constexpr float g_nearClip = 1.0f;
constexpr float g_farClip = 100.0f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // anonymous namespace

namespace nc::graphics
{
SpotLightSystem::SpotLightSystem(ShaderResourceBus* shaderResourceBus, uint32_t maxSpotLights, bool useShadows)
    : m_spotLightBuffer{shaderResourceBus->CreateStorageBuffer(sizeof(SpotLightData) * maxSpotLights, ShaderStage::Fragment | ShaderStage::Vertex, 8, 0, false)},
      m_useShadows{useShadows}
{
    m_spotLightData.reserve(maxSpotLights);
}

auto SpotLightSystem::Execute(uint32_t currentFrameIndex, MultiView<SpotLight, Transform> view) -> SpotLightState
{
    OPTICK_CATEGORY("SpotLightSystem::Execute", Optick::Category::Rendering);
    auto state = SpotLightState{};
    m_spotLightData.clear();

    auto lightsCount = 0u;
    for (const auto& [light, transform] : view)
    {
        lightsCount++;
        state.viewProjections.push_back(::CalculateLightViewProjectionMatrix(transform->TransformationMatrix()));
        m_spotLightData.emplace_back(state.viewProjections.back(),
                                     transform->Position(),
                                     m_useShadows,
                                     light->color,
                                     transform->Forward(),
                                     std::cos(light->innerAngle),
                                     std::cos(light->outerAngle));
    }

    m_spotLightBuffer.Bind(m_spotLightData, currentFrameIndex);
    if (m_useShadows && m_syncedLightsCount.at(currentFrameIndex) != lightsCount)
    {
        state.updateShadows = true;
    }
    m_syncedLightsCount.at(currentFrameIndex) = lightsCount;
    return state;
}

void SpotLightSystem::Clear() noexcept
{
    m_spotLightData.clear();
    m_spotLightBuffer.Clear();
    for (auto i : std::views::iota(0u, MaxFramesInFlight))
    {
        m_syncedLightsCount.at(i) = 0u;
    }
}
} // namespace nc::graphics
