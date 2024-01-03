#include "SkeletalAnimationShaderResource.h"
#include "graphics/api/vulkan/Initializers.h"
#include "graphics/shader_resource/SkeletalAnimationData.h"

namespace nc::graphics
{
constexpr uint32_t AvgBonesPerAnim = 30u;

SkeletalAnimationShaderResource::SkeletalAnimationShaderResource(uint32_t bindingSlot, GpuAllocator* allocator, ShaderDescriptorSets* descriptors, uint32_t maxSkeletalAnimations)
    : m_bindingSlot{bindingSlot},
      m_allocator{allocator},
      m_descriptors{descriptors},
      m_skeletalAnimationArrayBuffer{nullptr},
      m_maxAnimatedBones{AvgBonesPerAnim * maxSkeletalAnimations}
{
    Initialize();
}

void SkeletalAnimationShaderResource::Initialize()
{
    const auto bufferSize = static_cast<uint32_t>(sizeof(SkeletalAnimationData) * m_maxAnimatedBones);

    if (m_skeletalAnimationArrayBuffer == nullptr)
    {
        m_skeletalAnimationArrayBuffer = std::make_unique<WriteableBuffer<SkeletalAnimationData>>(m_allocator, bufferSize);
    }

    m_descriptors->RegisterDescriptor
    (
        m_bindingSlot,
        BindFrequency::per_frame,
        1,
        vk::DescriptorType::eStorageBuffer,
        vk::ShaderStageFlagBits::eVertex,
        vk::DescriptorBindingFlagBitsEXT()
    );

    m_descriptors->UpdateBuffer
    (
        BindFrequency::per_frame,
        m_skeletalAnimationArrayBuffer->GetBuffer(),
        bufferSize,
        1,
        vk::DescriptorType::eStorageBuffer,
        m_bindingSlot
    );
}

void SkeletalAnimationShaderResource::Update(const std::vector<SkeletalAnimationData>& data)
{
    m_skeletalAnimationArrayBuffer->Map(data);
}

void SkeletalAnimationShaderResource::Reset()
{
    m_skeletalAnimationArrayBuffer->Clear();
    Initialize();
}
} // namespace nc::graphics
