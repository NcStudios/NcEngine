#include "Lighting.h"
#include "ecs/Registry.h"
#include "graphics/GpuAllocator.h"
#include "graphics/GpuOptions.h"
#include "graphics/RenderGraph.h"
#include "graphics/Swapchain.h"
#include "graphics/renderpasses/RenderPass.h"
#include "graphics/shaders/ShaderDescriptorSets.h"
#include "graphics/shaders/ShaderResources.h"

namespace
{
auto CreateShadowMappingPass(vk::Device device, nc::graphics::GpuAllocator *allocator, nc::graphics::GpuOptions *gpuOptions, nc::graphics::Swapchain *swapchain, nc::Vector2 dimensions, uint32_t index) -> std::unique_ptr<nc::graphics::RenderPass>
{
    using namespace nc::graphics;
    auto id = ShadowMappingPassId + std::to_string(index);

    std::vector<AttachmentSlot> shadowAttachmentSlots{
        AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}};

    std::vector<Subpass> shadowSubpasses{
        Subpass{shadowAttachmentSlots[0]}};

    std::vector<std::unique_ptr<Attachment>> attachments;
    attachments.push_back(std::make_unique<Attachment>(device, allocator, dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = std::make_unique<RenderPass>(device, 0u, id, shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth);

    auto handles = std::vector<vk::ImageView>{renderPass->attachments[0]->view.get()};
    RegisterAttachments(device, renderPass.get(), std::move(handles), dimensions, 0);

    return std::move(renderPass);
}
}

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
    const auto countBeforeClearing = m_numShadowCasters;
    for (auto i = 0u; i < countBeforeClearing; ++i)
    {
        OnRemovePointLightConnection();
    }

    m_numShadowCasters = 0u;
}

void Lighting::Resize(RenderGraph* renderGraph, const Vector2& dimensions)
{
    m_renderGraph = renderGraph;
    m_dimensions = dimensions;

    const auto countBeforeClearing = m_numShadowCasters;
    m_numShadowCasters = 0u;
    for (auto i = 0u; i < countBeforeClearing; ++i)
    {
        OnAddPointLightConnection();
    }
}

void Lighting::OnAddPointLightConnection()
{
    const auto id = ShadowMappingPassId + std::to_string(m_numShadowCasters);
    m_renderGraph->Add(CreateShadowMappingPass(m_device, m_allocator, m_gpuOptions, m_swapchain, m_dimensions, m_numShadowCasters));
    m_renderGraph->RegisterShadowMappingTechnique(id, m_numShadowCasters);

    std::vector<ShadowMap> shadowMaps;
    shadowMaps.reserve(m_numShadowCasters + 1);
    for (auto i = 0u; i < m_numShadowCasters + 1; ++i)
    {
        auto currentPassId = ShadowMappingPassId + std::to_string(i);
        shadowMaps.push_back(ShadowMap{m_renderGraph->Acquire(currentPassId)->attachments[0]->view.get()});
    }
    m_shaderResources->shadowMapShaderResource.Update(std::vector<ShadowMap>{shadowMaps});
    m_numShadowCasters++;
}

void Lighting::OnRemovePointLightConnection()
{
    m_numShadowCasters--;
    const auto id = ShadowMappingPassId + std::to_string(m_numShadowCasters);
    m_renderGraph->UnregisterTechnique<ShadowMappingTechnique>(id);
    m_renderGraph->Remove(id);
}
}