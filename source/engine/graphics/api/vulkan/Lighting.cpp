#include "Lighting.h"
#include "RenderGraph.h"
#include "graphics/PointLight.h"

#include <algorithm>

namespace nc::graphics
{
Lighting::Lighting(Registry* registry,
                   RenderGraph* renderGraph)
    : m_renderGraph{renderGraph},
      m_onCommitPointLightConnection{registry->OnCommit<PointLight>().Connect([this](graphics::PointLight&){OnCommitPointLightConnection();})},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){OnRemovePointLightConnection();})}
{
}

void Lighting::Clear()
{
    m_renderGraph->ClearShadowPasses();
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
