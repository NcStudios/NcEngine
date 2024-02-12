#pragma once

#include "graphics/GraphicsConstants.h"

#include "vulkan/vk_mem_alloc.hpp"

#include <span>
#include <unordered_map>

namespace
{
    auto NullBinding = vk::DescriptorSetLayoutBinding(std::numeric_limits<uint32_t>::max(), vk::DescriptorType::eUniformBuffer, 0, vk::ShaderStageFlagBits::eAll);
}

namespace nc::graphics
{
    struct DescriptorSetLayout
    {
        vk::UniqueDescriptorSetLayout layout;
        std::vector<vk::DescriptorSetLayoutBinding> bindings = std::vector<vk::DescriptorSetLayoutBinding>{DefaultResourceSlotsPerShader, NullBinding};
        std::vector<vk::DescriptorBindingFlagsEXT> bindingFlags = std::vector<vk::DescriptorBindingFlagsEXT>{DefaultResourceSlotsPerShader};
    };

    struct DescriptorWrites
    {
        std::vector<vk::WriteDescriptorSet> writes = std::vector<vk::WriteDescriptorSet>{DefaultResourceSlotsPerShader};
    };

    struct PerFrameDescriptorSets
    {
        std::vector<vk::UniqueDescriptorSet> sets;
        std::vector<DescriptorWrites> writesPerSet;
        std::vector<bool> isDirty;
    };

    class ShaderDescriptorSets
    {
        public:
            ShaderDescriptorSets(vk::Device device);

            /* Resources attach themselves to a shader slot by registering themselves here. */
            void RegisterDescriptor(uint32_t frameIndex, uint32_t bindingSlot, uint32_t setIndex, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags);
            
            /* Called when the data in the image or buffer changes. */
            void UpdateImage(uint32_t frameIndex, uint32_t setIndex, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);
            void UpdateBuffer(uint32_t frameIndex, uint32_t setIndex, vk::Buffer buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);

            /* Called in the techniques to access and bind the descriptor set(s). */
            vk::DescriptorSetLayout* GetSetLayout(uint32_t setIndex);
            void BindSet(uint32_t frameIndex, uint32_t setIndex, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet);

        private:
            auto SetExists(uint32_t frameIndex, uint32_t setIndex) -> bool { return m_perFrameSets.at(frameIndex).sets.size() > setIndex; }
            auto GetSet(uint32_t frameIndex, uint32_t setIndex) -> vk::DescriptorSet* { return &m_perFrameSets.at(frameIndex).sets.at(setIndex).get(); }
            auto LayoutExists(uint32_t setIndex) -> bool { return m_layouts.size() > setIndex; }

            vk::Device m_device;
            vk::UniqueDescriptorPool m_pool;
            std::vector<PerFrameDescriptorSets> m_perFrameSets;
            std::vector<DescriptorSetLayout> m_layouts;
    };
}