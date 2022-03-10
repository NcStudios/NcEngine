#pragma once

#include "vulkan/vk_mem_alloc.hpp"
#include <span>

namespace nc::graphics
{
    class Graphics;

    enum class bind_frequency : uint8_t
    {
        per_frame
    };

    /* Represents the binding of a resource handle to the descriptor set's GPU memory */
    struct descriptor_write
    {
        /* Buffer write */
        descriptor_write(vk::WriteDescriptorSet inWrite, vk::DescriptorBufferInfo inBuffer)
            : write{inWrite},
              buffer{inBuffer},
              images{std::span<const vk::DescriptorImageInfo>()}
        {}

        /* Images write */
        descriptor_write(vk::WriteDescriptorSet inWrite, std::span<const vk::DescriptorImageInfo> inImages)
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
    struct descriptor_set
    {
        vk::UniqueDescriptorSet set;
        vk::UniqueDescriptorSetLayout layout;
        std::unordered_map<uint32_t, descriptor_write> writes;
        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> bindings;
        std::unordered_map<uint32_t, vk::DescriptorBindingFlagsEXT> bindingFlags;
        bool isDirty;
    };

    class shader_descriptor_sets
    {
        public:
            shader_descriptor_sets(Graphics* graphics);

            /* Shader resource services attach themselves to a shader slot by registering themselves here. */
            uint32_t register_descriptor(uint32_t bindingSlot, bind_frequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags);
            
            /* Called when the data in the image or buffer changes. */
            void update_image(bind_frequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);
            void update_buffer(bind_frequency bindFrequency, vk::Buffer* buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot);

            /* Called in the techniques to access and bind the descriptor set(s). */
            vk::DescriptorSetLayout* get_set_layout(bind_frequency bindFrequency);
            descriptor_set* get_set(bind_frequency bindFrequency);
            void bind_set(bind_frequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet);

            /* Called in ShaderResourceServices CTOR after each of the services have been initialized. */
            void create_set(bind_frequency bindFrequency);

        private:
            std::unordered_map<bind_frequency, descriptor_set> m_descriptorSets;
            Graphics* m_graphics;
    };
}