#include "RenderGraph.h"
#include "graphics/GraphicsUtilities.h"
#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/FrameManager.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/techniques/EnvironmentTechnique.h"
#include "graphics/api/vulkan/techniques/OutlineTechnique.h"
#include "graphics/api/vulkan/techniques/ParticleTechnique.h"
#include "graphics/api/vulkan/techniques/PbrTechnique.h"
#include "graphics/api/vulkan/techniques/ShadowMappingTechnique.h"
#include "graphics/api/vulkan/techniques/ToonTechnique.h"
#include "graphics/api/vulkan/techniques/UiTechnique.h"

#ifdef NC_EDITOR_ENABLED
#include "graphics/api/vulkan/techniques/WireframeTechnique.h"
#endif

#include "optick.h"

#include <array>
#include <ranges>
#include <string>

namespace
{
void SetViewportAndScissorFullWindow(vk::CommandBuffer* cmd, const nc::Vector2& dimensions)
{
    const auto viewport = vk::Viewport{0.0f, 0.0f, dimensions.x, dimensions.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{0, 0}, extent};
    cmd->setViewport(0, 1, &viewport);
    cmd->setScissor(0, 1, &scissor);
}

void SetViewportAndScissorAspectRatio(vk::CommandBuffer* cmd, const nc::Vector2& dimensions, const nc::Vector2& screenExtent)
{
    const auto viewport = vk::Viewport{(dimensions.x - screenExtent.x) / 2, (dimensions.y - screenExtent.y) / 2, screenExtent.x, screenExtent.y, 0.0f, 1.0f};
    const auto extent = vk::Extent2D{static_cast<uint32_t>(screenExtent.x), static_cast<uint32_t>(screenExtent.y)};
    const auto scissor = vk::Rect2D{vk::Offset2D{static_cast<int32_t>(dimensions.x - static_cast<float>(extent.width)) / 2, static_cast<int32_t>(dimensions.y - static_cast<float>(extent.height)) / 2}, extent};
    cmd->setViewport(0, 1, &viewport);
    cmd->setScissor(0, 1, &scissor);
}

auto CreateShadowMappingPass(const nc::graphics::vulkan::Device* device, nc::graphics::vulkan::GpuAllocator* allocator, nc::graphics::vulkan::Swapchain* swapchain, const nc::Vector2& dimensions, uint32_t shadowCasterIndex, uint32_t frameIndex) -> nc::graphics::vulkan::RenderPass
{
    using namespace nc::graphics::vulkan;

    const auto vkDevice = device->VkDevice();
    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 1>
    {
        AttachmentSlot
        {
            0,
            vk::Format::eD16Unorm,                                   // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal,         // Initial subpass layout
            vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal, // Final layout
            vk::AttachmentLoadOp::eClear,                            // Attachment load
            vk::AttachmentStoreOp::eStore,                           // Attachment store
            vk::AttachmentLoadOp::eDontCare,                         // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            vk::SampleCountFlagBits::e1                              // Sample count
        }
    };

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    auto attachment = std::vector<Attachment>{};
    attachment.push_back(Attachment(vkDevice, allocator, dimensions, true, shadowAttachmentSlots[0].numSamples, shadowAttachmentSlots[0].format));

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = RenderPass(vkDevice, ShadowMappingPassId + std::to_string(shadowCasterIndex), shadowAttachmentSlots, shadowSubpasses, std::move(attachment), size, ClearValueFlags::Depth);

    const auto views = std::array<vk::ImageView, 1>{renderPass.GetAttachmentView(0u)};
    renderPass.CreateFrameBuffers(views, dimensions, frameIndex);

    return renderPass;
}

auto CreateLitPass(const nc::graphics::vulkan::Device* device, nc::graphics::vulkan::GpuAllocator* allocator, nc::graphics::vulkan::Swapchain* swapchain, const nc::Vector2& dimensions) -> nc::graphics::vulkan::RenderPass
{
    using namespace nc::graphics::vulkan;

    const auto vkDevice = device->VkDevice();
    const auto& gpuOptions = device->GetGpuOptions();

    auto numSamples = gpuOptions.GetMaxSamplesCount();

    const auto litAttachmentSlots = std::array<AttachmentSlot, 3>
    {
        AttachmentSlot // Swapchain Image (Color)
        {
            0,
            swapchain->GetFormat(),                                  // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eColorAttachmentOptimal,                // Initial subpass layout
            vk::ImageLayout::eColorAttachmentOptimal,                // Final layout
            vk::AttachmentLoadOp::eClear,                            // Attachment load
            vk::AttachmentStoreOp::eStore,                           // Attachment store
            vk::AttachmentLoadOp::eDontCare,                         // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            numSamples                                               // Sample count
        },
        AttachmentSlot // Depth
        {
            1,
            gpuOptions.GetDepthFormat(),                             // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal,         // Initial subpass layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal,         // Final layout
            vk::AttachmentLoadOp::eClear,                            // Attachment load
            vk::AttachmentStoreOp::eDontCare,                        // Attachment store
            vk::AttachmentLoadOp::eClear,                            // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            numSamples                                               // Sample count
        },
        AttachmentSlot // MSAA Color Resolve
        {
            2,
            swapchain->GetFormat(),                                  // Image format
            vk::ImageLayout::eUndefined,                             // Initial layout
            vk::ImageLayout::eColorAttachmentOptimal,                // Initial subpass layout
            vk::ImageLayout::ePresentSrcKHR,                         // Final layout
            vk::AttachmentLoadOp::eDontCare,                         // Attachment load
            vk::AttachmentStoreOp::eStore,                           // Attachment store
            vk::AttachmentLoadOp::eDontCare,                         // Stencil load
            vk::AttachmentStoreOp::eDontCare,                        // Stencil store
            vk::SampleCountFlagBits::e1                              // Sample count
        },
    };

    const auto litSubpasses = std::array<Subpass, 1>
    {
        Subpass{litAttachmentSlots[0], litAttachmentSlots[1], litAttachmentSlots[2]}
    };

    std::vector<Attachment> attachments;
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, litAttachmentSlots[1].numSamples, litAttachmentSlots[1].format)); // Depth Stencil
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, false, litAttachmentSlots[0].numSamples, litAttachmentSlots[0].format)); // Color Buffer

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = RenderPass(vkDevice, LitPassId, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto &colorImageViews = swapchain->GetColorImageViews();
    auto depthImageView = renderPass.GetAttachmentView(0);
    auto colorResolveView = renderPass.GetAttachmentView(1);

    uint32_t index = 0;
    for (auto &imageView : colorImageViews)
    {
        std::vector<vk::ImageView> imageViews
        {
            colorResolveView, // Color Resolve View
            depthImageView, // Depth View
            imageView.get()
        };
        renderPass.CreateFrameBuffers(imageViews, dimensions, index++);
    }
    return renderPass;
}

auto CreateLitPasses(const nc::graphics::vulkan::Device* device,
                     nc::graphics::vulkan::GpuAllocator* alloc,
                     nc::graphics::vulkan::Swapchain* swapchain,
                     const nc::Vector2& dimensions)
{
    return [&] <size_t... N> (std::index_sequence<N...>)
    {
        return std::array{((void)N, CreateLitPass(device, alloc, swapchain, dimensions))...};
    }(std::make_index_sequence<nc::graphics::MaxFramesInFlight>());
}
}

namespace nc::graphics::vulkan
{
RenderGraph::RenderGraph(FrameManager* frameManager, Registry* registry, const Device* device, Swapchain* swapchain, GpuAllocator* gpuAllocator, ShaderBindingManager* shaderBindingManager, Vector2 dimensions, uint32_t maxLights)
    : m_frameManager{frameManager},
      m_device{device},
      m_swapchain{swapchain},
      m_gpuAllocator{gpuAllocator},
      m_shaderBindingManager{shaderBindingManager},
      m_shadowMappingPasses{},
      m_litPass{CreateLitPasses(m_device, m_gpuAllocator, m_swapchain, dimensions)},
      m_postProcessImageViews{},
      m_dummyShadowMap{Attachment(m_device->VkDevice(), m_gpuAllocator, Vector2{1.0f, 1.0f}, true, vk::SampleCountFlagBits::e1, vk::Format::eD16Unorm)},
      m_onDescriptorSetsChanged{m_shaderBindingManager->OnResourceLayoutChanged().Connect(this, &RenderGraph::SetDescriptorSetLayoutsDirty)},
      m_onCommitOmniLight{registry->OnCommit<PointLight>().Connect([this](graphics::PointLight&){IncrementShadowPassCount(true);})},
      m_onRemoveOmniLight{registry->OnRemove<PointLight>().Connect([this](Entity){DecrementShadowPassCount(true);})},
      m_onCommitUniLight{registry->OnCommit<SpotLight>().Connect([this](graphics::SpotLight&){IncrementShadowPassCount(false);})},
      m_onRemoveUniLight{registry->OnRemove<SpotLight>().Connect([this](Entity){DecrementShadowPassCount(false);})},
      m_dimensions{dimensions},
      m_screenExtent{},
      m_omniDirLightCount{},
      m_uniDirLightCount{},
      m_maxLights{maxLights},
      m_isDescriptorSetLayoutsDirty{std::array<bool, MaxFramesInFlight>{true, true}}
{
    auto dummyView = m_dummyShadowMap.view.get();
    m_postProcessImageViews.emplace(PostProcessImageType::ShadowMap, PostProcessViews
    {
        .perFrameViews = std::array<std::vector<vk::ImageView>, MaxFramesInFlight>
        {
            std::vector<vk::ImageView>(m_maxLights, dummyView),
            std::vector<vk::ImageView>(m_maxLights, dummyView)
        }
    });
}

void RenderGraph::SinkPostProcessImages()
{
    OPTICK_CATEGORY("RenderGraph::SinkPostProcessImages", Optick::Category::Rendering);

    auto* currentFrame = m_frameManager->CurrentFrameContext();
    auto& perFrameShadowMapViews = m_postProcessImageViews.at(PostProcessImageType::ShadowMap).perFrameViews[currentFrame->Index()];
    perFrameShadowMapViews = std::vector<vk::ImageView>(m_maxLights, m_dummyShadowMap.view.get());
    auto& shadowPasses = m_shadowMappingPasses[currentFrame->Index()];

    for (auto i : std::views::iota(0u, m_omniDirLightCount))
    {
        perFrameShadowMapViews[i] = shadowPasses[i].GetAttachmentView(0u);
    }

    for (auto i : std::views::iota(m_omniDirLightCount, m_omniDirLightCount + m_uniDirLightCount))
    {
        perFrameShadowMapViews[i] = shadowPasses[i].GetAttachmentView(0u);
    }
}

auto RenderGraph::GetPostProcessImages(PostProcessImageType imageType) -> const std::vector<vk::ImageView>&
{
    return m_postProcessImageViews.at(imageType).perFrameViews.at(m_frameManager->CurrentFrameContext()->Index());
}

void RenderGraph::CommitResourceLayout()
{
    auto* currentFrame = m_frameManager->CurrentFrameContext();
    const auto frameIndex = currentFrame->Index();

    if (!m_isDescriptorSetLayoutsDirty.at(frameIndex))
    {
        return;
    }

    auto& perFramePasses =  m_shadowMappingPasses.at(frameIndex);
    perFramePasses.clear();

    for (auto i : std::views::iota(0u, m_omniDirLightCount))
    {
        perFramePasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, i, frameIndex));
        perFramePasses[i].ClearTechniques();
        perFramePasses[i].RegisterShadowMappingTechnique(m_device->VkDevice(), m_shaderBindingManager, i, true);
    }

    for (auto i : std::views::iota(m_omniDirLightCount, m_omniDirLightCount + m_uniDirLightCount))
    {
        perFramePasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, i, frameIndex));
        perFramePasses[i].ClearTechniques();
        perFramePasses[i].RegisterShadowMappingTechnique(m_device->VkDevice(), m_shaderBindingManager, i, false);
    }

    auto& litPass = m_litPass.at(frameIndex);
    litPass.ClearTechniques();

    #ifdef NC_EDITOR_ENABLED
    litPass.RegisterTechnique<WireframeTechnique>(*m_device, m_shaderBindingManager);
    #endif

    litPass.RegisterTechnique<EnvironmentTechnique>(*m_device, m_shaderBindingManager);
    litPass.RegisterTechnique<PbrTechnique>(*m_device, m_shaderBindingManager);
    litPass.RegisterTechnique<ToonTechnique>(*m_device, m_shaderBindingManager);
    litPass.RegisterTechnique<OutlineTechnique>(*m_device, m_shaderBindingManager);
    litPass.RegisterTechnique<ParticleTechnique>(*m_device, m_shaderBindingManager);
    litPass.RegisterTechnique<UiTechnique>(*m_device, m_shaderBindingManager);

    m_isDescriptorSetLayoutsDirty.at(frameIndex) = false;
}

void RenderGraph::RecordDrawCallsOnBuffer(const PerFrameRenderState &frameData, uint32_t frameBufferIndex, const Vector2& dimensions, const Vector2& screenExtent)
{
    OPTICK_CATEGORY("RenderGraph::RecordDrawCallsOnBuffer", Optick::Category::Rendering);

    auto* currentFrame = m_frameManager->CurrentFrameContext();
    const auto frameIndex = currentFrame->Index();
    const auto cmd = currentFrame->CommandBuffer();

    SetViewportAndScissorFullWindow(cmd, dimensions);

    for (auto& shadowMappingPass : m_shadowMappingPasses.at(frameIndex))
    {
        shadowMappingPass.Begin(cmd, frameBufferIndex);
        shadowMappingPass.Execute(cmd, frameData, frameIndex);
        shadowMappingPass.End(cmd);
    }

    SetViewportAndScissorAspectRatio(cmd, dimensions, screenExtent);

    auto& litPass = m_litPass.at(frameIndex);
    litPass.Begin(cmd, frameBufferIndex);
    litPass.Execute(cmd, frameData, frameIndex);
    litPass.End(cmd);
}

void RenderGraph::Resize(const Vector2& dimensions)
{
    for (auto frameIndex : std::views::iota(0u, MaxFramesInFlight))
    {
        m_litPass[frameIndex] = CreateLitPass(m_device, m_gpuAllocator, m_swapchain, dimensions);
        auto& shadowPasses = m_shadowMappingPasses[frameIndex];
        auto& shadowMapViews = m_postProcessImageViews.at(PostProcessImageType::ShadowMap).perFrameViews[frameIndex];
        shadowPasses.clear();
        shadowMapViews.clear();

        for (auto shadowPassIndex : std::views::iota(0u, m_omniDirLightCount))
        {
            shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, shadowPassIndex, frameIndex));
            shadowMapViews.emplace_back(shadowPasses.back().GetAttachmentView(0u));
        }

        for (auto shadowPassIndex : std::views::iota(m_omniDirLightCount, m_omniDirLightCount +  m_uniDirLightCount))
        {
            shadowPasses.push_back(CreateShadowMappingPass(m_device, m_gpuAllocator, m_swapchain, m_dimensions, shadowPassIndex, frameIndex));
            shadowMapViews.emplace_back(shadowPasses.back().GetAttachmentView(0u));
        }

        m_isDescriptorSetLayoutsDirty[frameIndex] = true;
    }
}

void RenderGraph::IncrementShadowPassCount(bool isOmniDirectional)
{
    NC_ASSERT(m_omniDirLightCount + m_uniDirLightCount < m_maxLights, "Tried to add a light source when max lights are registered.");

    if (isOmniDirectional)
    {
        m_omniDirLightCount++;
    }
    else
    {
        m_uniDirLightCount++;
    }
    
    SetDescriptorSetLayoutsDirty(DescriptorSetLayoutsChanged{});
}

void RenderGraph::ClearShadowPasses() noexcept
{
    m_omniDirLightCount = 0u;
    m_uniDirLightCount = 0u;
    SetDescriptorSetLayoutsDirty(DescriptorSetLayoutsChanged{});
}

void RenderGraph::DecrementShadowPassCount(bool isOmniDirectional)
{
    if (isOmniDirectional)
    {
        NC_ASSERT(m_omniDirLightCount > 0, "Tried to remove a light source when none are registered.");
        m_omniDirLightCount--;
    }
    else
    {
        NC_ASSERT(m_uniDirLightCount > 0, "Tried to remove a light source when none are registered.");
        m_uniDirLightCount--;
    }

    SetDescriptorSetLayoutsDirty(DescriptorSetLayoutsChanged{});
}

void RenderGraph::SetDescriptorSetLayoutsDirty(const DescriptorSetLayoutsChanged&)
{
    for (auto& isDirty :  m_isDescriptorSetLayoutsDirty)
    {
        isDirty = true;
    }
}
} // namespace nc::graphics::vulkan
