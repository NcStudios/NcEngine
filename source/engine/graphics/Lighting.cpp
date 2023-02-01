#include "Lighting.h"
#include "ecs/Registry.h"
#include "GpuAllocator.h"
#include "GpuOptions.h"
#include "RenderGraph.h"
#include "renderpasses/RenderPass.h"
#include "shaders/ShaderDescriptorSets.h"
#include "shaders/ShaderResources.h"
#include "Swapchain.h"

namespace nc::graphics
{
Lighting::Lighting(Registry* registry, 
                   vk::Device device, 
                   GpuAllocator* allocator, 
                   GpuOptions* gpuOptions, 
                   Swapchain* swapchain,
                   RenderGraph* renderGraph,
                   ShaderDescriptorSets* shaderDescriptorSets,
                   ShaderResources* shaderResources,
                   Vector2 dimensions)
    : m_registry{registry}, 
      m_device{device}, 
      m_allocator{allocator},
      m_gpuOptions{gpuOptions},
      m_swapchain{swapchain},
      m_renderGraph{renderGraph},
      m_shaderDescriptorSets{shaderDescriptorSets},
      m_shaderResources{shaderResources},
      m_dimensions{dimensions},
      
      m_onAddPointLightConnection{registry->OnAdd<PointLight>().Connect([this](graphics::PointLight&){OnAddPointLightConnection();})},
      m_onRemovePointLightConnection{registry->OnRemove<PointLight>().Connect([this](Entity){OnRemovePointLightConnection();})},
      m_numShadowCasters{0u}
{
}

void Lighting::Clear()
{
    auto countBeforeClearing = m_numShadowCasters;
    for (auto i = 0u; i < countBeforeClearing; ++i)
    {
        OnRemovePointLightConnection();
    }

    m_numShadowCasters = 0u;
}

void Lighting::Resize(const Vector2& dimensions)
{
    auto countBeforeClearing = m_numShadowCasters;
    Clear();
    for (auto i = 0u; i < countBeforeClearing; ++i)
    {
        OnAddPointLightConnection();
    }
}

void Lighting::OnAddPointLightConnection()
{
    auto id = ShadowMappingPassId + std::to_string(m_numShadowCasters);
    m_renderGraph->Add(CreateShadowMappingPass(m_device, m_allocator, m_gpuOptions, m_swapchain, m_dimensions, m_numShadowCasters));
    m_renderGraph->RegisterShadowMappingTechnique(id, m_numShadowCasters);

    std::vector<ShadowMap> shadowMaps;
    shadowMaps.reserve(m_numShadowCasters + 1);
    for (auto i = 0u; i < m_numShadowCasters + 1; ++i)
    {
        auto currentPassId = ShadowMappingPassId + std::to_string(i);
        shadowMaps.push_back(ShadowMap{m_renderGraph->Acquire(currentPassId).attachments[0]->view.get()});
    }
    m_shaderResources->shadowMapShaderResource.Update(std::vector<ShadowMap>{shadowMaps});
    m_numShadowCasters++;
}

void Lighting::OnRemovePointLightConnection()
{
    m_numShadowCasters--;
    auto id = ShadowMappingPassId + std::to_string(m_numShadowCasters);
    m_renderGraph->UnregisterTechnique<ShadowMappingTechnique>(id);
    m_renderGraph->Remove(id);
}
}