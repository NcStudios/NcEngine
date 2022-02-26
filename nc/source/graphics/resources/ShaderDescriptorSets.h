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

    struct DescriptorSet
    {
        vk::UniqueDescriptorSet set;
        vk::UniqueDescriptorSetLayout layout;
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
    };

    class ShaderDescriptorSets
    {
        public:
            ShaderDescriptorSets(Graphics* graphics);

            uint32_t RegisterDescriptor(BindFrequency bindFrequency, vk::Buffer* buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages);
            uint32_t RegisterDescriptor(BindFrequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagsEXT bindingFlags = vk::DescriptorBindingFlagsEXT());
            void UpdateDescriptor(BindFrequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);

            vk::DescriptorSetLayout* GetDescriptorSetLayout(BindFrequency bindFrequency);
            void Bind(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t setCount);
            DescriptorSet* GetSet(BindFrequency bindFrequency);
            uint32_t GetSetLayoutCount(std::span<BindFrequency> bindFrequencies);

        private:
            void UpdateSet(BindFrequency bindFrequency);

            std::unordered_map<BindFrequency, DescriptorSet> m_descriptorSets;
            std::vector<vk::DescriptorBindingFlagsEXT> m_bindingFlags;
            Graphics* m_graphics;
    };
}