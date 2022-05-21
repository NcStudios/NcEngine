#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include <span>

namespace nc::graphics
{
    class Base;

    enum class BindFrequency : uint8_t
    {
        per_frame
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
            ShaderDescriptorSets(Base* base);

            /* Shader resource services attach themselves to a shader slot by registering themselves here. */
            uint32_t RegisterDescriptor(uint32_t bindingSlot, BindFrequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags);
            
            /* Called when the data in the image or buffer changes. */
            void UpdateImage(BindFrequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);
            void UpdateBuffer(BindFrequency bindFrequency, vk::Buffer buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);

            /* Called in the techniques to access and bind the descriptor set(s). */
            vk::DescriptorSetLayout* GetSetLayout(BindFrequency bindFrequency);
            DescriptorSet* GetSet(BindFrequency bindFrequency);
            void BindSet(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet);

            /* Called in ShaderResourceServices CTOR after each of the services have been initialized. */
            void CreateSet(BindFrequency bindFrequency);

        private:
            vk::UniqueDescriptorPool m_renderingDescriptorPool;
            std::unordered_map<BindFrequency, DescriptorSet> m_descriptorSets;

            /** @todo needs only Device & PadBufferOffset... */
            Base* m_base;
    };
}