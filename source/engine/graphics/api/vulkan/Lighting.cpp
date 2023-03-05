#include "Lighting.h"
#include "FrameManager.h"
#include "GpuAllocator.h"
#include "RenderGraph.h"
#include "Swapchain.h"
#include "core/GpuOptions.h"
#include "ecs/Registry.h"
#include "renderpasses/RenderPass.h"
#include "shaders/ShaderDescriptorSets.h"
#include "shaders/ShaderResources.h"

#include <array>

namespace
{
inline static const std::string ShadowMappingPassId = "Shadow Mapping Pass";
}

namespace nc::graphics
{
Lighting::Lighting(Registry* registry,
                   vk::Device device,
                   GpuAllocator* allocator,
                   Swapchain* swapchain,
                   RenderGraph* renderGraph,
                   ShaderDescriptorSets* shaderDescriptorSets,
                   ShaderResources* shaderResources,
                   Vector2 dimensions)
    : m_registry{registry}, 
      m_device{device}, 
      m_allocator{allocator},
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

void Lighting::Resize(const Vector2& dimensions)
{
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
    m_renderGraph->AddRenderPass(CreateShadowMappingPass(m_dimensions, m_numShadowCasters));

    std::vector<ShadowMap> shadowMaps;
    shadowMaps.reserve(m_numShadowCasters + 1);
    for (auto i = 0u; i < m_numShadowCasters + 1; ++i)
    {
        shadowMaps.push_back(ShadowMap{ m_renderGraph->GetRenderPass(m_ids.at(i)).GetAttachmentView(0) });
    }
    m_shaderResources->shadowMapShaderResource.Update(std::vector<ShadowMap>{shadowMaps});
    m_numShadowCasters++;
}

void Lighting::OnRemovePointLightConnection()
{
    m_numShadowCasters--;
    m_renderGraph->RemoveRenderPass(m_ids.back());
    m_ids.pop_back();
}

auto Lighting::CreateShadowMappingPass(nc::Vector2 dimensions, uint32_t index) -> nc::graphics::RenderPass
{
    using namespace nc::graphics;
    m_ids.emplace_back(ShadowMappingPassId + std::to_string(index));

    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 1>
    {
        AttachmentSlot{0, AttachmentType::ShadowDepth, vk::Format::eD16Unorm, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::SampleCountFlagBits::e1}
    };

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    std::vector<Attachment> attachments;
    const auto numConcurrentAttachments = m_swapchain->GetColorImageViews().size();
    for (auto i = 0u; i < numConcurrentAttachments; i++)
    {
        attachments.push_back(Attachment(m_device, m_allocator, dimensions, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm));
    }

    const auto size = AttachmentSize{dimensions, m_swapchain->GetExtent()};
    auto renderPass = RenderPass(m_device, 0u, m_ids.back(), shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth);

    for (auto i = 0u; i < numConcurrentAttachments; i++)
    {
        const auto views = std::array<vk::ImageView, 1>{renderPass.GetAttachmentView(i)};
        renderPass.RegisterAttachmentViews(views, dimensions, i);
    }

    renderPass.RegisterShadowMappingTechnique(m_device, m_shaderDescriptorSets, index);
    return renderPass;
}
}