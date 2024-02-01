#pragma once

#include "vulkan/vk_mem_alloc.hpp"

#include <span>
#include <unordered_map>

namespace nc::graphics
{
    enum class DescriptorScope : uint8_t
    {
        Global,
        PerObject,
        Lighting
    };

    /* Represents the binding of a resource handle to the descriptor set's GPU memory */
    struct DescriptorWrite
    {
        /* Buffer write */
        DescriptorWrite(vk::WriteDescriptorSet inWrite, vk::DescriptorBufferInfo inBuffer)
            : write{inWrite},
              buffer{inBuffer},
              images{std::span<const vk::DescriptorImageInfo>()}
        {}

        /* Images write */
        DescriptorWrite(vk::WriteDescriptorSet inWrite, std::span<const vk::DescriptorImageInfo> inImages)
            : write{ inWrite },
              buffer{ vk::DescriptorBufferInfo() },
              images{ inImages }
        {}

        vk::WriteDescriptorSet write;
        vk::DescriptorBufferInfo buffer;
        std::span<const vk::DescriptorImageInfo> images;
    };

    /* All of the components required for a descriptor set.
    *  The unordered_map keys are the binding slots for each descriptor in the set. 
    *  (The binding slots correspond with the shader slots.) */
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
            ShaderDescriptorSets(vk::Device device);

            /* Shader resource services attach themselves to a shader slot by registering themselves here. */
            uint32_t RegisterDescriptor(uint32_t bindingSlot, DescriptorScope descriptorScope, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags);
            
            /* Called when the data in the image or buffer changes. */
            void UpdateImage(DescriptorScope descriptorScope, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);
            void UpdateBuffer(DescriptorScope descriptorScope, vk::Buffer buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);

            /* Called in the techniques to access and bind the descriptor set(s). */
            vk::DescriptorSetLayout* GetSetLayout(DescriptorScope descriptorScope);
            DescriptorSet* GetSet(DescriptorScope descriptorScope);
            void BindSet(DescriptorScope descriptorScope, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet);

            /* Called in ShaderResources CTOR after each of the services have been initialized. */
            void CreateSet(DescriptorScope descriptorScope);

        private:
            vk::UniqueDescriptorPool m_renderingDescriptorPool;
            std::unordered_map<DescriptorScope, DescriptorSet> m_descriptorSets;
            vk::Device m_device;
    };
}