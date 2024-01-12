#include "Lighting.h"
#include "RenderGraph.h"
#include "graphics/PointLight.h"
#include "shaders/ShaderResources.h"

#include <algorithm>

namespace nc::graphics
{
Lighting::Lighting(Registry* registry,
                   RenderGraph* renderGraph,
                   ShaderResources* shaderResources)
    : m_renderGraph{renderGraph},
      m_shaderResources{shaderResources},
      m_onAddPointLightConnection{registry->OnAdd<PointLight>().Connect([this](graphics::PointLight&){OnAddPointLightConnection();})},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){OnRemovePointLightConnection();})}
{
}

void Lighting::Clear()
{
    m_renderGraph->ClearShadowPasses();
}

void Lighting::Resize()
{
    std::vector<ShadowMap> shadowMaps;
    shadowMaps.reserve(MaxLights);

    auto& shadowMapPasses = m_renderGraph->GetShadowPasses();
    std::transform(shadowMapPasses.cbegin(), shadowMapPasses.cend(), std::back_inserter(shadowMaps), [](auto&& pass)
    {
        return ShadowMap{pass.GetAttachmentView(0)};
    });

    m_shaderResources->shadowMapShaderResource.Update(std::vector<ShadowMap>{shadowMaps});
}

void Lighting::OnAddPointLightConnection()
{
    m_renderGraph->IncrementShadowPassCount();

    std::vector<ShadowMap> shadowMaps;
    shadowMaps.reserve(MaxLights);

    auto& shadowMapPasses = m_renderGraph->GetShadowPasses();
    std::transform(shadowMapPasses.cbegin(), shadowMapPasses.cend(), std::back_inserter(shadowMaps), [](auto&& pass)
    {
        return ShadowMap{pass.GetAttachmentView(0)};
    });

    m_shaderResources->shadowMapShaderResource.Update(std::vector<ShadowMap>{shadowMaps});
}

void Lighting::OnRemovePointLightConnection()
{
    m_renderGraph->DecrementShadowPassCount();
}
} // namespace nc::graphics
