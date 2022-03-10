#include "shader_descriptor_sets.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    shader_descriptor_sets::shader_descriptor_sets(Graphics* graphics)
        : m_descriptorSets{},
          m_graphics{graphics}
    {
        m_descriptorSets.reserve(1);
        m_descriptorSets.emplace(bind_frequency::per_frame, descriptor_set{});
    }

    uint32_t shader_descriptor_sets::register_descriptor(uint32_t bindingSlot, bind_frequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags)
    {
        auto* descriptorSet = get_set(bindFrequency);

        /* Add binding */
        auto bindingsIt = descriptorSet->bindings.find(bindingSlot);
        if (bindingsIt != descriptorSet->bindings.end()) bindingsIt->second = CreateDescriptorSetLayoutBinding(bindingSlot, descriptorCount, descriptorType, shaderStages);
        else descriptorSet->bindings.emplace(bindingSlot, CreateDescriptorSetLayoutBinding(bindingSlot, descriptorCount, descriptorType, shaderStages));

        /* Add binding flag */
        auto flagsIt = descriptorSet->bindingFlags.find(bindingSlot);
        if (flagsIt != descriptorSet->bindingFlags.end()) flagsIt->second = bindingFlags;
        else descriptorSet->bindingFlags.emplace(bindingSlot, bindingFlags);

        return bindingSlot;
    }

    vk::DescriptorSetLayout* shader_descriptor_sets::get_set_layout(bind_frequency bindFrequency)
    {
        return &get_set(bindFrequency)->layout.get();
    }

    void shader_descriptor_sets::bind_set(bind_frequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet)
    {
        auto* descriptorSet = get_set(bindFrequency);

        /* Only update the descriptor sets if they have changed since last bind. */
        if (descriptorSet->isDirty)
        {
            std::vector<vk::WriteDescriptorSet> writes;
            writes.reserve(descriptorSet->writes.size());

            for (auto& kv : descriptorSet->writes)
            {
                kv.second.write.setDstSet(descriptorSet->set.get());
                writes.push_back(kv.second.write);
            }
            m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
            descriptorSet->isDirty = false;
        }

        /* Bind the set to the pipeline */
        cmd->bindDescriptorSets(bindPoint, pipelineLayout, firstSet, 1, &descriptorSet->set.get(), 0, 0);
    }

    void shader_descriptor_sets::create_set(bind_frequency bindFrequency)
    {
        auto* descriptorSet = get_set(bindFrequency);

        /* Grab the bindings from the unordered map. */
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        bindings.reserve(descriptorSet->bindings.size());
        for (auto& kv : descriptorSet->bindings)
        {
            bindings.push_back(kv.second);
        }

        /* Grab the binding flags from the unordered map. */
        std::vector<vk::DescriptorBindingFlagsEXT> bindingFlags;
        bindingFlags.reserve(descriptorSet->bindingFlags.size());
        for (auto& kv : descriptorSet->bindingFlags)
        {
            bindingFlags.push_back(kv.second);
        }

        descriptorSet->layout = CreateDescriptorSetLayout(m_graphics, bindings, bindingFlags);
        descriptorSet->set = CreateDescriptorSet(m_graphics, m_graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr(), 1, &descriptorSet->layout.get());
    }

    void shader_descriptor_sets::update_image(bind_frequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot)
    {
        auto* descriptorSet = get_set(bindFrequency);

        vk::WriteDescriptorSet write{};
        write.setDstBinding(bindingSlot);
        write.setDstArrayElement(0);
        write.setDescriptorType(descriptorType);
        write.setDescriptorCount(descriptorCount);
        write.setPBufferInfo(0);
        write.setPImageInfo(imageInfos.data());

        /* Add or update the write for the image descriptor. */
        auto it = descriptorSet->writes.find(bindingSlot);
        if (it != descriptorSet->writes.end()) it->second = descriptor_write(write, imageInfos);
        else descriptorSet->writes.emplace(bindingSlot, descriptor_write(write, imageInfos));

        descriptorSet->isDirty = true;
    }

    void shader_descriptor_sets::update_buffer(bind_frequency bindFrequency, vk::Buffer* buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot)
    {
        auto* descriptorSet = get_set(bindFrequency);

        uint32_t range = 0;
        range = descriptorType != vk::DescriptorType::eUniformBuffer && descriptorType != vk::DescriptorType::eStorageBuffer ? setSize : m_graphics->GetBasePtr()->PadBufferOffsetAlignment(setSize, descriptorType);

        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = *buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = range;

        vk::WriteDescriptorSet write{};
        write.setDstBinding(bindingSlot);
        write.setDstArrayElement(0);
        write.setDescriptorType(descriptorType);
        write.setDescriptorCount(descriptorCount);
        write.setPImageInfo(0);

        /* Add or update the write for the buffer descriptor. */
        auto it = descriptorSet->writes.find(bindingSlot);
        if (it != descriptorSet->writes.end())
        {
            it->second = descriptor_write(write, bufferInfo);
            it->second.write.setPBufferInfo(&it->second.buffer); /* Update the pointer */
        }
        else
        {
            auto [pos, success] = descriptorSet->writes.emplace(bindingSlot, descriptor_write(write, bufferInfo));
            pos->second.write.setPBufferInfo(&pos->second.buffer);  /* Update the pointer */
        }

        descriptorSet->isDirty = true;
    }

    descriptor_set* shader_descriptor_sets::get_set(bind_frequency bindFrequency)
    {
        return &m_descriptorSets.at(bindFrequency);
    }
}