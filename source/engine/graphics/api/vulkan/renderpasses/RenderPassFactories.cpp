#include "RenderPassFactories.h"
#include "graphics/api/vulkan/core/Device.h"
#include "graphics/api/vulkan/GpuAllocator.h"
#include "graphics/api/vulkan/PerFrameGpuContext.h"
#include "graphics/api/vulkan/ShaderBindingManager.h"
#include "graphics/api/vulkan/Swapchain.h"
#include "graphics/api/vulkan/pipelines/ShadowMappingOmniPipeline.h"
#include "graphics/api/vulkan/pipelines/ShadowMappingUniPipeline.h"
#include "graphics/api/vulkan/renderpasses/Attachment.h"

#include <span>

namespace
{
auto InitializeShadowMappingCubeMap(uint32_t faceSideLength, size_t id) -> nc::asset::CubeMapWithId
{
    constexpr auto NumChannels = 4u;
    constexpr auto NumFaces = 6u;
    return nc::asset::CubeMapWithId
    {
        .cubeMap = nc::asset::CubeMap
        {
            .faceSideLength = faceSideLength,
            .pixelData = std::vector<unsigned char>(faceSideLength * faceSideLength * NumChannels * NumFaces)
        },
        .id = id
    };
}
}
namespace nc::graphics::vulkan
{
auto CreateShadowMappingPassOmni(const Device* device,
                                 GpuAllocator* allocator,
                                 ShaderBindingManager* shaderBindingManager,
                                 std::vector<vk::ImageView> sourceViews) -> RenderPass
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
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, true, true, shadowAttachmentSlots.at(1).numSamples, shadowAttachmentSlots.at(1).format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc));

    const auto size = AttachmentSize{ShadowMapDimensions, shadowMapExtent};
    auto renderPass = RenderPass(vkDevice, shadowAttachmentSlots, shadowSubpasses, std::move(attachments), size, ClearValueFlags::Depth | ClearValueFlags::Color);

    auto attachmentViews = std::array<vk::ImageView, 2>{};
    attachmentViews.at(1) = renderPass.GetAttachmentView(0u); // Depth attachment

    for (auto cubeMapFaceView : sourceViews)
    {
        attachmentViews.at(0) = cubeMapFaceView;
        renderPass.CreateFrameBuffer(attachmentViews, ShadowMapDimensions);
    }
    renderPass.RegisterPipeline<ShadowMappingOmniPipeline>(device, shaderBindingManager);

    return renderPass;
}

/**
 * Create a cube map texture
 *  ID: shadowCubeMap.image
 *  Format: R32SFloat
 *  Layers: 6
 *  Layout: ShaderReadOnlyOptimal
 * 
 * Create one cube map image view
 *  ID: shadowCubeMap.view
 *  Format: R32SFloat
 *  Layers 6
 * 
 * Create six cube map image views
 *  ID: shadowCubeMapFaceImageViews
 *  Format: R32SFloat
 *  Layers: 1
 *  BaseArrayLayer: 0  - 5
 * 
 * Create a renderpass
 *  Create two attachment descriptions
 *    ID: osAttachments[0]
 *    Format: R32SFloat
 * 
 *    ID: osAttachments[1]
 *    Format: depthFormat
 *  
 *  Create two attachment references
 *    ID: colorReference
 *    Attachment: 0
 *    Layout: ColorAttachmentOptimal
 *    
 *    ID: depthReference
 *    Attachment: 1
 *    Layout: DepthStencilAttachmentOptimal
 * 
 *  Create one subpass description
 *    ID: subpass
 *    ColorAttachmentCount: 1
 *    ColorAttachment: colorReference
 *    DepthAttachment: depthReference
 *
 *  Create Renderpass
 *    ID: offscreenPass.renderPass
 *    AttachmentCount: 2
 *    SubPassCount: 1
 *    pAttachments: osAttachments (colorReference, depthReference)
 *
 * Create Frame Buffer Image
 *  ID: offscreenPass.depth.image
 *  Usage: VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
 *  ArrayLayers: 1
 * 
 * Create Frame Buffer Image View
 *  ID: offscreenPass.depth.view
 *  Format: depthFormat
 *  Image: offscreenPass.depth.image
 *  Layout: VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
 *  LayerCount: 1
 * 
 * Create six FrameBuffers
 *  ID: offscreenPass.frameBuffers[0 - 5]
 *  ImageView[i]: shadowCubeMapFaceImageViews[i]
 *  ImageView[1]: offscreenPass.depth.view
 *  AttachmentCount: 2
 * 
 *  
 */

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
    attachments.push_back(Attachment(vkDevice, allocator, ShadowMapDimensions, true, false, shadowAttachmentSlots[0].numSamples, shadowAttachmentSlots[0].format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled));

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
    renderPass.RegisterPipeline<ShadowMappingUniPipeline>(device, shaderBindingManager);

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
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, true, false, litAttachmentSlots[1].numSamples, litAttachmentSlots[1].format, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled)); // Depth Stencil
    attachments.push_back(Attachment(vkDevice, allocator, dimensions, false, false, litAttachmentSlots[0].numSamples, litAttachmentSlots[0].format, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment)); // AA Color Resolve Buffer

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

auto CreateDummyShadowMaps(size_t count) -> std::vector<nc::asset::CubeMapWithId>
{
    auto shadowMaps = std::vector<nc::asset::CubeMapWithId>{};
    shadowMaps.reserve(count);
    std::generate_n(std::back_inserter(shadowMaps), count, []()
    {
        static size_t index = 0u; 
        return InitializeShadowMappingCubeMap(static_cast<uint32_t>(ShadowMapDimensions.x), index++);
    });

    return shadowMaps;
}
} // namespace nc::graphics::vulkan