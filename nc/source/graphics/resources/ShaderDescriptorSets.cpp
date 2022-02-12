#include "ShaderDescriptorSets.h"
#include "graphics/Base.h"
#include "graphics/Graphics.h"
#include "graphics/Initializers.h"

namespace nc::graphics
{
    ShaderDescriptorSets::ShaderDescriptorSets(Graphics* graphics)
        : m_descriptorSets{},
        m_bindingFlags{vk::DescriptorBindingFlagsEXT()},
        m_graphics{graphics}
    {
        m_descriptorSets.reserve(1);
        m_descriptorSets.emplace(BindFrequency::PerFrame, DescriptorSet{});
    }

    uint32_t ShaderDescriptorSets::RegisterDescriptor(BindFrequency bindFrequency, vk::Buffer* buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages)
    {
        auto* descriptorSet = GetSet(bindFrequency);
        auto descriptorSlot = static_cast<uint32_t>(descriptorSet->bindings.size());
        descriptorSet->bindings.push_back(CreateDescriptorSetLayoutBinding(descriptorSlot, descriptorCount, descriptorType, shaderStages));
        UpdateSet(bindFrequency, vk::DescriptorBindingFlagBitsEXT());

        vk::DescriptorBufferInfo objectsDataBufferInfo;
        objectsDataBufferInfo.buffer = *buffer;
        objectsDataBufferInfo.offset = 0; // @TODO Probably need offset
        objectsDataBufferInfo.range = setSize;

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(descriptorType);
        write.setDescriptorCount(descriptorCount);
        write.setDstSet(descriptorSet->set.get());
        write.setPBufferInfo(&objectsDataBufferInfo);
        write.setPImageInfo(0);

        m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);

        return descriptorSlot;
    }

    uint32_t ShaderDescriptorSets::RegisterDescriptor(BindFrequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagsEXT bindingFlags)
    {
        auto* descriptorSet = GetSet(bindFrequency);
        auto descriptorSlot = static_cast<uint32_t>(descriptorSet->bindings.size());
        descriptorSet->bindings.push_back(CreateDescriptorSetLayoutBinding(descriptorSlot, descriptorCount, descriptorType, shaderStages));
        UpdateSet(bindFrequency, bindingFlags);

        UpdateDescriptor(bindFrequency, imageInfos, descriptorCount, descriptorType);

        return descriptorSlot;
    }

    void ShaderDescriptorSets::UpdateDescriptor(BindFrequency bindFrequency, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType)
    {
        auto* descriptorSet = GetSet(bindFrequency);

        vk::WriteDescriptorSet write{};
        write.setDstBinding(0);
        write.setDstArrayElement(0);
        write.setDescriptorType(descriptorType);
        write.setDescriptorCount(descriptorCount);
        write.setDstSet(descriptorSet->set.get());
        write.setPBufferInfo(0);
        write.setPImageInfo(imageInfos.data());

        m_graphics->GetBasePtr()->GetDevice().updateDescriptorSets(1, &write, 0, nullptr);
    }

    vk::DescriptorSetLayout* ShaderDescriptorSets::GetDescriptorSetLayout(BindFrequency bindFrequency)
    {
        return &GetSet(bindFrequency)->layout.get();
    }

    void ShaderDescriptorSets::Bind(BindFrequency bindFrequency, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t setCount)
    {
        cmd->bindDescriptorSets(bindPoint, pipelineLayout, firstSet, setCount, &GetSet(bindFrequency)->set.get(), 0, 0);
    }

    void ShaderDescriptorSets::UpdateSet(BindFrequency bindFrequency, vk::DescriptorBindingFlagsEXT bindingFlags)
    {
        auto* descriptorSet = GetSet(bindFrequency);
        descriptorSet->layout = CreateDescriptorSetLayout(m_graphics, descriptorSet->bindings, m_bindingFlags);
        descriptorSet->set = CreateDescriptorSet(m_graphics, m_graphics->GetBasePtr()->GetRenderingDescriptorPoolPtr(), 1, &descriptorSet->layout.get());
    }

    DescriptorSet* ShaderDescriptorSets::GetSet(BindFrequency bindFrequency)
    {
        return &m_descriptorSets.at(bindFrequency);
    }
}