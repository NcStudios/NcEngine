#include "ShaderDescriptorSets.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    ShaderDescriptorSets::ShaderDescriptorSets(Graphics* graphics)
        : m_descriptorSets{},
          m_graphics{graphics}
    {
        m_descriptorSets.reserve(1);
        m_descriptorSets.emplace(BindFrequency::PerFrame, DescriptorSet{});
    }

    uint32_t ShaderDescriptorSets::RegisterDescriptor(uint32_t bindingSlot, BindFrequency bindFrequency, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags)
    {
        auto* descriptorSet = GetSet(bindFrequency);

        // Add binding
        auto bindingsIt = descriptorSet->bindings.find(bindingSlot);
        if (bindingsIt != descriptorSet->bindings.end()) 
            bindingsIt->second = CreateDescriptorSetLayoutBinding(bindingSlot, descriptorCount, descriptorType, shaderStages);
        else 
            descriptorSet->bindings.emplace(bindingSlot, CreateDescriptorSetLayoutBinding(bindingSlot, descriptorCount, descriptorType, shaderStages));

        // Add binding flag
        auto flagsIt = descriptorSet->bindingFlags.find(bindingSlot);
        if (flagsIt != descriptorSet->bindingFlags.end())
            flagsIt->second = bindingFlags;
        else
            descriptorSet->bindingFlags.emplace(bindingSlot, bindingFlags);

        return bindingSlot;
    }

    vk::DescriptorSetLayout* ShaderDescriptorSets::GetSetLayout(BindFrequency bindFrequency)
    {
        return &GetSet(bindFrequency)->layout.get();
    }

    void ShaderDescriptorSets::BindSet(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t setCount)
    {
        auto* descriptorSet = GetSet(bindFrequency);

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

        cmd->bindDescriptorSets(bindPoint, pipelineLayout, firstSet, 1, &GetSet(bindFrequency)->set.get(), 0, 0);
    }

    void ShaderDescriptorSets::CreateSet(BindFrequency bindFrequency)
    {
        auto* descriptorSet = GetSet(bindFrequency);

        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        bindings.reserve(descriptorSet->bindings.size());
        for (auto& kv : descriptorSet->bindings)
        {
            bindings.push_back(kv.second);
        }

        std::vector<vk::DescriptorBindingFlagsEXT> bindingFlags;
        bindingFlags.reserve(descriptorSet->bindingFlags.size());
        for (auto& kv : descriptorSet->bindingFlags)
        {
            bindingFlags.push_back(kv.second);
        }

        descriptorSet->layout = CreateDescriptorSetLayout(m_graphics, bindings, bindingFlags);
        descriptorSet->set = CreateDescriptorSet(m_graphics, m_graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr(), 1, &descriptorSet->layout.get());
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

        auto it = descriptorSet->writes.find(bindingSlot);
        if (it != descriptorSet->writes.end())
        {
            it->second = DescriptorWrite(write, imageInfos);
        }
        else
        {
            auto [pos, success] = descriptorSet->writes.emplace(bindingSlot, DescriptorWrite(write, imageInfos));
        }

        descriptorSet->isDirty = true;
    }

    void ShaderDescriptorSets::UpdateBuffer(BindFrequency bindFrequency, vk::Buffer* buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot)
    {
        auto* descriptorSet = GetSet(bindFrequency);

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

        auto it = descriptorSet->writes.find(bindingSlot);
        if (it != descriptorSet->writes.end())
        {
            it->second = DescriptorWrite(write, bufferInfo);
            it->second.write.setPBufferInfo(&it->second.buffer);
        }
        else
        {
            auto [pos, success] = descriptorSet->writes.emplace(bindingSlot, DescriptorWrite(write, bufferInfo));
            pos->second.write.setPBufferInfo(&pos->second.buffer);
        }

        descriptorSet->isDirty = true;
    }

    DescriptorSet* ShaderDescriptorSets::GetSet(BindFrequency bindFrequency)
    {
        return &m_descriptorSets.at(bindFrequency);
    }
}