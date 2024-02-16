#include "Lighting.h"
#include "RenderGraph.h"
#include "graphics/PointLight.h"
#include "shaders/ShaderResources.h"
#include "shaders/ShadowMapShaderResource.h"

#include <algorithm>

namespace
{
void InitializeAllShadowMaps(nc::graphics::RenderGraph* renderGraph, nc::graphics::ShaderResources* shaderResources)
{
    auto& shadowMapPasses = renderGraph->GetShadowPasses();
    std::vector<nc::graphics::ShadowMapData> shadowMaps;
    shadowMaps.reserve(shadowMapPasses.size());
    std::transform(shadowMapPasses.cbegin(), shadowMapPasses.cend(), std::back_inserter(shadowMaps), [](auto&& pass)
    {
        return nc::graphics::ShadowMapData{pass.GetAttachmentView(0)};
    });

    shaderResources->shadowMapShaderResource.Update(std::move(std::vector<nc::graphics::ShadowMapData>{shadowMaps}));
}
}

namespace nc::graphics
{
Lighting::Lighting(Registry* registry,
                   RenderGraph* renderGraph,
                   ShaderResources* shaderResources)
    : m_renderGraph{renderGraph},
      m_shaderResources{shaderResources},
      m_onCommitPointLightConnection{registry->OnCommit<PointLight>().Connect([this](graphics::PointLight&){OnCommitPointLightConnection();})},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){OnRemovePointLightConnection();})}
{
    InitializeAllShadowMaps(m_renderGraph, m_shaderResources);
}

void Lighting::Clear()
{
    m_renderGraph->ClearShadowPasses();
}

void Lighting::Resize()
{
    InitializeAllShadowMaps(m_renderGraph, m_shaderResources);
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
