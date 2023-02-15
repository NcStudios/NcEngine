#pragma once

#include "graphics/techniques/ITechnique.h"
#include "ncmath/Vector.h"

#include <memory>
#include <span>
#include <vector>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{

class GpuAllocator;
class GpuOptions;
class Swapchain;

using ClearValueFlags_t = uint8_t;

struct ClearValueFlags
{
    static constexpr ClearValueFlags_t None = 0;
    static constexpr ClearValueFlags_t Color = 1 << 1;
    static constexpr ClearValueFlags_t Depth = 1 << 2;
};

enum class AttachmentType : uint8_t
{
    Color,
    Depth,
    ShadowDepth,
    Resolve
};

struct FrameBuffer
{
    uint32_t index{};
    std::span<const vk::ImageView> attachmentHandles{};
    vk::UniqueFramebuffer frameBuffer;
};

struct Attachment
{
    Attachment(vk::Device device, GpuAllocator* allocator, Vector2 dimensions, bool isDepthStencil, vk::SampleCountFlagBits numSamples, vk::Format depthFormat);
    GpuAllocation<vk::Image> image;
    vk::UniqueImageView view;
};

struct AttachmentSlot
{
    AttachmentSlot(uint32_t attachmentIndex, AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp, vk::SampleCountFlagBits numSamples);
    vk::AttachmentDescription description;
    vk::AttachmentReference reference;
    AttachmentType type;
};

struct Subpass
{
    Subpass(const AttachmentSlot &colorAttachment, const AttachmentSlot &depthAttachment, const AttachmentSlot &resolveAttachment);
    explicit Subpass(const AttachmentSlot &depthAttachment);
    vk::SubpassDescription description;
    std::vector<vk::SubpassDependency> dependencies{};
};

struct AttachmentSize
{
    Vector2 dimensions;
    vk::Extent2D extent;
};

struct RenderPass
{
    RenderPass(vk::Device device,
               uint8_t priority_,
               std::string uid_,
               std::vector<AttachmentSlot> attachmentSlots,
               std::vector<Subpass> subpasses,
               std::vector<std::unique_ptr<Attachment>> attachments_,
               const AttachmentSize& size,
               ClearValueFlags_t clearFlags_);
    uint8_t priority;
    std::string uid;
    vk::UniqueRenderPass renderPass;
    AttachmentSize attachmentSize;
    ClearValueFlags_t clearFlags;
    std::vector<std::unique_ptr<ITechnique>> techniques{};
    std::vector<std::unique_ptr<Attachment>> attachments{};
    std::vector<std::unique_ptr<FrameBuffer>> frameBuffers{};
};

auto GetFrameBuffer(nc::graphics::RenderPass *renderPass, uint32_t index) -> nc::graphics::FrameBuffer*;
void RegisterAttachments(vk::Device device, nc::graphics::RenderPass *renderPass, std::span<const vk::ImageView> attachmentHandles, const nc::Vector2 &dimensions, uint32_t index);
} // namespace nc::graphics
