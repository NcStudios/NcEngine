#pragma once

#include "graphics/techniques/ITechnique.h"
#include "ncmath/Vector.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <array>
#include <memory>
#include <span>
#include <vector>

/** @todo As more stuff gets pulled into here, consider splitting this up into multiple files. */

namespace nc::graphics
{
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

struct AttachmentSlot
{
    AttachmentSlot(uint32_t attachmentIndex, AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp,
                   vk::AttachmentStoreOp storeOp, vk::SampleCountFlagBits numSamples);

    vk::AttachmentDescription description;
    vk::AttachmentReference reference;
    AttachmentType type;
};

struct Subpass
{
    /** Create lit subpass */
    Subpass(const AttachmentSlot& colorAttachment, const AttachmentSlot& depthAttachment, const AttachmentSlot& resolveAttachment);

    /** Create shadow mapping subpass */
    Subpass(const AttachmentSlot& depthAttachment);

    vk::SubpassDescription description;
    std::vector<vk::SubpassDependency> dependencies;
};

struct RenderTargetSize
{
    Vector2 dimensions;
    vk::Extent2D extent;
};

struct RenderPass
{
    RenderPass(std::span<const AttachmentSlot> attachmentSlots, std::span<const Subpass> subpasses,
               vk::Device device, const RenderTargetSize& size, std::string uid, ClearValueFlags_t clearFlags);

    vk::UniqueRenderPass renderpass;
    std::vector<std::unique_ptr<ITechnique>> techniques;
    RenderTargetSize renderTargetSize;
    std::string uid;
    ClearValueFlags_t clearFlags;
};
} // namespace nc::graphics
