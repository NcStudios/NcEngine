#pragma once

#include <vulkan/vulkan.hpp>

namespace nc::graphics
{
struct PerFrameRenderState;
namespace vulkan
{
class ITechnique
{
    public:
        virtual ~ITechnique() = default;

        virtual bool CanBind(const PerFrameRenderState& frameData) = 0;
        virtual void Bind(uint32_t frameIndex, vk::CommandBuffer* cmd) = 0;

        virtual bool CanRecord(const PerFrameRenderState& frameData) = 0;
        virtual void Record(vk::CommandBuffer* cmd, const PerFrameRenderState& frameData) = 0;
};
} // namespace nc::graphics
} // namespace vulkan
