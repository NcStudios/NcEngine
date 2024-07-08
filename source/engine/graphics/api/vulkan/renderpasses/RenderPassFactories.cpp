#include "RenderPassFactories.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/pipelines/ShadowMappingPipeline.h"
#include "graphics/api/vulkan/renderpasses/Attachment.h"
#include "ncengine/asset/AssetData.h"

#include <span>

// namespace
// {
// auto InitializeShadowMappingCubeMap()
// {

// }
// }

namespace nc::graphics::vulkan
{
auto CreateShadowMappingPassOmni(const Device* device,
                                 GpuAllocator* allocator,
                                 ShaderBindingManager* shaderBindingManager,
                                 std::span<const vk::ImageView> sourceViews) -> RenderPass
{
    const auto vkDevice = device->VkDevice();
    const auto& gpuOptions = device->GetGpuOptions();
    const auto shadowAttachmentSlots = std::array<AttachmentSlot, 2>
    {
        AttachmentSlot // Color blit source
        {
            0,
            vk::Format::eR32Sfloat,                   // Image format
            vk::ImageLayout::eShaderReadOnlyOptimal,  // Initial layout
            vk::ImageLayout::eColorAttachmentOptimal, // Initial subpass layout
            vk::ImageLayout::eShaderReadOnlyOptimal,  // Final layout
            vk::AttachmentLoadOp::eClear,             // Attachment load
            vk::AttachmentStoreOp::eStore,            // Attachment store
            vk::AttachmentLoadOp::eDontCare,          // Stencil load
            vk::AttachmentStoreOp::eDontCare,         // Stencil store
            vk::SampleCountFlagBits::e1               // Sample count
        },
        AttachmentSlot // Depth image
        {
            1,
            gpuOptions.GetDepthFormat(),                     // Image format
            vk::ImageLayout::eDepthStencilAttachmentOptimal, // Initial layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal, // Initial subpass layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal, // Final layout
            vk::AttachmentLoadOp::eClear,                    // Attachment load
            vk::AttachmentStoreOp::eStore,                   // Attachment store
            vk::AttachmentLoadOp::eDontCare,                 // Stencil load
            vk::AttachmentStoreOp::eDontCare,                // Stencil store
            vk::SampleCountFlagBits::e1                      // Sample count
        }
    };

    auto shadowMapExtent = vk::Extent2D{static_cast<uint32_t>(ShadowMapDimensions.x), static_cast<uint32_t>(ShadowMapDimensions.y)};

    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots.at(0), shadowAttachmentSlots.at(1)}};

    auto attachments = std::vector<Attachment>{};
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, false, shadowAttachmentSlots.at(0).numSamples, shadowAttachmentSlots.at(0).format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc)); // Attachment to be blitted to a cube face
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, true, shadowAttachmentSlots.at(1).numSamples, shadowAttachmentSlots.at(1).format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc));

    const auto size = AttachmentSize{ShadowMapDimensions, shadowMapExtent};
    auto renderPass = RenderPass(vkDevice, shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto attachmentViews = std::array<vk::ImageView, 2>{};
    attachmentViews.at(1) = renderPass.GetAttachmentView(1u);

    for (auto cubeMapFaceView : sourceViews)
    {
        attachmentViews.at(0) = cubeMapFaceView;
        renderPass.CreateFrameBuffer(attachmentViews, ShadowMapDimensions);

    }
    renderPass.RegisterPipeline<ShadowMappingPipeline>(device, shaderBindingManager);

    return renderPass;
}

auto CreateShadowMappingPass(const Device* device,
                             GpuAllocator* allocator,
                             ShaderBindingManager* shaderBindingManager) -> RenderPass
{
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

    auto shadowMapExtent = vk::Extent2D{static_cast<uint32_t>(ShadowMapDimensions.x), static_cast<uint32_t>(ShadowMapDimensions.y)};
    const auto shadowSubpasses = std::array<Subpass, 1>{Subpass{shadowAttachmentSlots[0]}};

    auto attachments = std::vector<Attachment>{};
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, true, shadowAttachmentSlots[0].numSamples, shadowAttachmentSlots[0].format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled));

    auto sinkViews = std::vector<vk::ImageView>{attachments.at(0).view.get()};
    auto renderPass = RenderPass(vkDevice,
                                 shadowAttachmentSlots,
                                 shadowSubpasses,
                                 std::move(attachments),
                                 AttachmentSize{ShadowMapDimensions, shadowMapExtent},
                                 ClearValueFlags::Depth,
                                 nc::graphics::RenderPassSinkType::UniDirShadowMap,
                                 std::move(sinkViews),
                                 vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal,
                                 0u);

    const auto attachmentViews = std::array<vk::ImageView, 1>{renderPass.GetAttachmentView(0u)};
    renderPass.CreateFrameBuffer(attachmentViews, ShadowMapDimensions);
    renderPass.RegisterPipeline<ShadowMappingPipeline>(device, shaderBindingManager);

    return renderPass;
}

auto CreateLitPass(const Device* device, GpuAllocator* allocator, Swapchain* swapchain, const nc::Vector2& dimensions) -> RenderPass
{
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
        AttachmentSlot // AA Color Resolve
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
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, litAttachmentSlots[1].numSamples, litAttachmentSlots[1].format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled)); // Depth Stencil
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, false, litAttachmentSlots[0].numSamples, litAttachmentSlots[0].format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment)); // AA Color Resolve Buffer

    const auto size = AttachmentSize{dimensions, swapchain->GetExtent()};
    auto renderPass = RenderPass(vkDevice, litAttachmentSlots, litSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto swapchainImageViews = swapchain->GetSwapchainImageViews();
    for (auto& swapchainImageView : swapchainImageViews) // We need a framebuffer with all three views (depth, AA resolve, swapchain) per swapchain image view.
    {
        auto attachmentViews = std::array<vk::ImageView, 3>
        {
            renderPass.GetAttachmentView(1), // Color Resolve View
            renderPass.GetAttachmentView(0), // Depth View
            swapchainImageView // Swapchain Image at index <swapchainImageIndex>
        };
        renderPass.CreateFrameBuffer(attachmentViews, dimensions);
    }

    return renderPass;
}
} // namespace nc::graphics::vulkan