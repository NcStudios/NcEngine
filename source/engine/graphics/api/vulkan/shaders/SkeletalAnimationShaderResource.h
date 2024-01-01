#pragma once

#include "ShaderDescriptorSets.h"
#include "graphics/api/vulkan/buffers/WriteableBuffer.h"
#include "graphics/shader_resource/ShaderResourceService.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class GpuAllocator;
struct SkeletalAnimationData;

class SkeletalAnimationShaderResource : public IShaderResource<SkeletalAnimationData>
{
    public:
        SkeletalAnimationShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors);

        void Initialize() override;
        void Update(const std::vector<SkeletalAnimationData>& data) override;
        void Reset() override;

    private:
        uint32_t m_bindingSlot;
        GpuAllocator* m_allocator;
        ShaderDescriptorSets* m_descriptors;
        std::unique_ptr<WriteableBuffer<SkeletalAnimationData>> m_skeletalAnimationArrayBuffer;
};
} // namespace nc::graphics
