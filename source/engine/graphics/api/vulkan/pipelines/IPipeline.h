#pragma once

#include <vulkan/vulkan.hpp>

namespace nc::graphics
{
struct PerFrameRenderState;
struct PerFrameInstanceData;

namespace vulkan
{
class IPipeline
{
    public:
        virtual ~IPipeline() = default;

        virtual void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) = 0;
        virtual void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData, const PerFrameInstanceData& instanceData) = 0;
};
} // namespace nc::graphics
} // namespace vulkan
