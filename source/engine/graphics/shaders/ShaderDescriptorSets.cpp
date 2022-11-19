#include "ShaderDescriptorSets.h"
#include "graphics/Initializers.h"

namespace
{
    auto CreateRenderingDescriptorPool(vk::Device device) -> vk::UniqueDescriptorPool
    {
        std::array<vk::DescriptorPoolSize, 4> renderingPoolSizes =
        {
            vk::DescriptorPoolSize { vk::DescriptorType::eSampler, 10 },
            vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, 1000 },
            vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, 10 },
            vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, 10 }
        };
        
        vk::DescriptorPoolCreateInfo renderingDescriptorPoolInfo = {};
        renderingDescriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
        renderingDescriptorPoolInfo.setMaxSets(1000);
        renderingDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(renderingPoolSizes.size()));
        renderingDescriptorPoolInfo.setPPoolSizes(renderingPoolSizes.data());

        return device.createDescriptorPoolUnique(renderingDescriptorPoolInfo);
    }
}

namespace nc::graphics
{
    ShaderDescriptorSets::ShaderDescriptorSets(vk::Device device)
        : m_renderingDescriptorPool{CreateRenderingDescriptorPool(device)},
          m_descriptorSets{},
          m_device{device}
    {
        m_descriptorSets.reserve(1);
        m_descriptorSets.emplace(BindFrequency::per_frame, DescriptorSet{});
    }

    uint32_t ShaderDescriptorSets::RegisterDescriptor(uint32_t bindingSlot, BindFrequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags)
    {
        auto* descriptorSet = GetSet(bindFrequency);

        /* Add binding */
        auto bindingsIt = descriptorSet->bindings.find(bindingSlot);
        if (bindingsIt != descriptorSet->bindings.end())
        {
            bindingsIt->second = vk::DescriptorSetLayoutBinding{bindingSlot, descriptorType, descriptorCount, shaderStages, nullptr};
        }
        else
        {
            descriptorSet->bindings.emplace(bindingSlot, vk::DescriptorSetLayoutBinding(bindingSlot, descriptorType, descriptorCount, shaderStages));
        }

        /* Add binding flag */
        auto flagsIt = descriptorSet->bindingFlags.find(bindingSlot);
        if (flagsIt != descriptorSet->bindingFlags.end()) flagsIt->second = bindingFlags;
        else descriptorSet->bindingFlags.emplace(bindingSlot, bindingFlags);

        return bindingSlot;
    }

    vk::DescriptorSetLayout* ShaderDescriptorSets::GetSetLayout(BindFrequency bindFrequency)
    {
        return &GetSet(bindFrequency)->layout.get();
    }

    void ShaderDescriptorSets::BindSet(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet)
    {
        auto* descriptorSet = GetSet(bindFrequency);

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

            m_device.updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
            descriptorSet->isDirty = false;
        }

        /* Bind the set to the pipeline */
        cmd->bindDescriptorSets(bindPoint, pipelineLayout, firstSet, 1, &descriptorSet->set.get(), 0, 0);
    }

    void ShaderDescriptorSets::CreateSet(BindFrequency bindFrequency)
    {
        auto* descriptorSet = GetSet(bindFrequency);

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

        descriptorSet->layout = CreateDescriptorSetLayout(m_device, bindings, bindingFlags);
        descriptorSet->set = CreateDescriptorSet(m_device, &m_renderingDescriptorPool.get(), 1, &descriptorSet->layout.get());
    }

    void ShaderDescriptorSets::UpdateImage(BindFrequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot)
    {
        auto* descriptorSet = GetSet(bindFrequency);

        vk::WriteDescriptorSet write{};
        write.setDstBinding(bindingSlot);
        write.setDstArrayElement(0);
        write.setDescriptorType(descriptorType);
        write.setDescriptorCount(descriptorCount);
        write.setPBufferInfo(0);
        write.setPImageInfo(imageInfos.data());

        /* Add or update the write for the image descriptor. */
        auto it = descriptorSet->writes.find(bindingSlot);
        if (it != descriptorSet->writes.end()) it->second = DescriptorWrite(write, imageInfos);
        else descriptorSet->writes.emplace(bindingSlot, DescriptorWrite(write, imageInfos));

        descriptorSet->isDirty = true;
    }

    void ShaderDescriptorSets::UpdateBuffer(BindFrequency bindFrequency, vk::Buffer buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot)
    {
        auto* descriptorSet = GetSet(bindFrequency);

        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = setSize;

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
            it->second = DescriptorWrite(write, bufferInfo);
            it->second.write.setPBufferInfo(&it->second.buffer); /* Update the pointer */
        }
        else
        {
            auto [pos, success] = descriptorSet->writes.emplace(bindingSlot, DescriptorWrite(write, bufferInfo));
            pos->second.write.setPBufferInfo(&pos->second.buffer);  /* Update the pointer */
        }

        descriptorSet->isDirty = true;
    }

    DescriptorSet* ShaderDescriptorSets::GetSet(BindFrequency bindFrequency)
    {
        return &m_descriptorSets.at(bindFrequency);
    }
}