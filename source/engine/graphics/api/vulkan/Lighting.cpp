#include "Lighting.h"
#include "RenderGraph.h"
#include "graphics/PointLight.h"
#include "shaders/ShaderResources.h"
#include "shaders/ShadowMapShaderResource.h"

#include <algorithm>

namespace nc::graphics
{
Lighting::Lighting(Registry* registry,
                   RenderGraph* renderGraph,
                   ShaderResources* shaderResources,
                   uint32_t maxPointLights)
    : m_renderGraph{renderGraph},
      m_shaderResources{shaderResources},
      m_onCommitPointLightConnection{registry->OnCommit<PointLight>().Connect([this](graphics::PointLight&){OnCommitPointLightConnection();})},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){OnRemovePointLightConnection();})},
      m_maxPointLights{maxPointLights}
{
    std::vector<ShadowMapData> shadowMaps;
    shadowMaps.reserve(m_maxPointLights);

    auto& shadowMapPasses = m_renderGraph->GetShadowPasses();
    std::transform(shadowMapPasses.cbegin(), shadowMapPasses.cend(), std::back_inserter(shadowMaps), [](auto&& pass)
    {
        return ShadowMapData{pass.GetAttachmentView(0)};
    });

    m_shaderResources->shadowMapShaderResource.Update(std::vector<ShadowMapData>{shadowMaps});
}

void Lighting::Clear()
{
    m_renderGraph->ClearShadowPasses();
}

void Lighting::Resize()
{
    std::vector<ShadowMapData> shadowMaps;
    shadowMaps.reserve(m_maxPointLights);

    auto& shadowMapPasses = m_renderGraph->GetShadowPasses();
    std::transform(shadowMapPasses.cbegin(), shadowMapPasses.cend(), std::back_inserter(shadowMaps), [](auto&& pass)
    {
        return ShadowMapData{pass.GetAttachmentView(0)};
    });

    m_shaderResources->shadowMapShaderResource.Update(std::vector<ShadowMapData>{shadowMaps});
}

void Lighting::OnCommitPointLightConnection()
{
    m_renderGraph->IncrementShadowPassCount();
}

void Lighting::OnRemovePointLightConnection()
{
    m_renderGraph->DecrementShadowPassCount();
}
} // namespace nc::graphics
