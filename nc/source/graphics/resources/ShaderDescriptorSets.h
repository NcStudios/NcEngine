#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include <span>

namespace nc::graphics
{
    class Graphics;

    enum class BindFrequency : uint8_t
    {
        PerFrame
    };

    struct DescriptorWrite
    {
        // Buffer Write
        DescriptorWrite(vk::WriteDescriptorSet inWrite, vk::DescriptorBufferInfo inBuffer)
            : write{inWrite},
              buffer{inBuffer},
              images{std::span<const vk::DescriptorImageInfo>()}
        {}

        // Images Write
        DescriptorWrite(vk::WriteDescriptorSet inWrite, std::span<const vk::DescriptorImageInfo> inImages)
            : write{ inWrite },
              buffer{ vk::DescriptorBufferInfo() },
              images{ inImages }
        {}

        vk::WriteDescriptorSet write;
        vk::DescriptorBufferInfo buffer;
        std::span<const vk::DescriptorImageInfo> images;
    };

    struct DescriptorSet
    {
        vk::UniqueDescriptorSet set;
        vk::UniqueDescriptorSetLayout layout;
        std::unordered_map<uint32_t, DescriptorWrite> writes;
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;
        std::unordered_map<uint32_t, vk::DescriptorBindingFlagsEXT> bindingFlags;
        bool isDirty;
    };

    class ShaderDescriptorSets
    {
        public:
            ShaderDescriptorSets(Graphics* graphics);

            uint32_t RegisterDescriptor(uint32_t bindingSlot, BindFrequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags);
            
            void UpdateImage(BindFrequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);
            void UpdateBuffer(BindFrequency bindFrequency, vk::Buffer* buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);

            vk::DescriptorSetLayout* GetSetLayout(BindFrequency bindFrequency);
            DescriptorSet* GetSet(BindFrequency bindFrequency);
            void CreateSet(BindFrequency bindFrequency);
            void BindSet(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t setCount);

        private:
            std::unordered_map<BindFrequency, DescriptorSet> m_descriptorSets;
            Graphics* m_graphics;
    };
}