#include "ShaderDescriptorSets.h"
#include "graphics/api/vulkan/Initializers.h"

#include "ncutility/NcError.h"

namespace
{
vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(vk::Device device, std::span<const vk::DescriptorSetLayoutBinding> layoutBindings, std::span<vk::DescriptorBindingFlagsEXT> bindingFlags)
{
    vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{};
    extendedInfo.setPNext(nullptr);
    extendedInfo.setBindingCount(static_cast<uint32_t>(layoutBindings.size()));
    extendedInfo.setPBindingFlags(bindingFlags.data());

    vk::DescriptorSetLayoutCreateInfo setInfo{};
    setInfo.setBindingCount(static_cast<uint32_t>(layoutBindings.size()));
    setInfo.setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool);
    setInfo.setPNext(&extendedInfo);
    setInfo.setPBindings(layoutBindings.data());

    return device.createDescriptorSetLayoutUnique(setInfo);
}

vk::UniqueDescriptorSet CreateDescriptorSet(vk::Device device, vk::DescriptorPool* descriptorPool, uint32_t descriptorSetCount, vk::DescriptorSetLayout* descriptorSetLayout)
{
    vk::DescriptorSetAllocateInfo allocationInfo{};
    allocationInfo.setPNext(nullptr);
    allocationInfo.setDescriptorPool(*descriptorPool);
    allocationInfo.setDescriptorSetCount(descriptorSetCount);
    allocationInfo.setPSetLayouts(descriptorSetLayout);

    // @todo: return the vector rather than the indiviual item, don't use move in return values
    return std::move(device.allocateDescriptorSetsUnique(allocationInfo)[0]);
}

auto CreateRenderingDescriptorPool(vk::Device device) -> vk::UniqueDescriptorPool
{
    std::array<vk::DescriptorPoolSize, 4> renderingPoolSizes =
    {
        vk::DescriptorPoolSize { vk::DescriptorType::eSampledImage, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, 1000 },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer, 10 },
        vk::DescriptorPoolSize { vk::DescriptorType::eUniformBuffer, 10 }
    };
    
    vk::DescriptorPoolCreateInfo renderingDescriptorPoolInfo = {};
    renderingDescriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);
    renderingDescriptorPoolInfo.setMaxSets(10);
    renderingDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(renderingPoolSizes.size()));
    renderingDescriptorPoolInfo.setPPoolSizes(renderingPoolSizes.data());

    return device.createDescriptorPoolUnique(renderingDescriptorPoolInfo);
}
}

namespace nc::graphics
{
    ShaderDescriptorSets::ShaderDescriptorSets(vk::Device device)
        : m_device{device},
          m_pool{CreateRenderingDescriptorPool(device)},
          m_perFrameSets{MaxFramesInFlight},
          m_layouts{}
    {
    }

    void ShaderDescriptorSets::RegisterDescriptor(uint32_t frameIndex, uint32_t bindingSlot, uint32_t setIndex, uint32_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags)
    {
        NC_ASSERT(bindingSlot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");

        if (!LayoutExists(setIndex))
        {
            m_layouts.emplace_back();
        }

        auto* layout = &m_layouts.at(setIndex);
        if (layout->bindings.size() <= bindingSlot)
        {
            layout->bindings.resize(std::min(static_cast<uint32_t>(layout->bindings.size()) + DefaultResourceSlotsPerShader, MaxResourceSlotsPerShader));
            layout->bindingFlags.resize(std::min(static_cast<uint32_t>(layout->bindingFlags.size()) + DefaultResourceSlotsPerShader, MaxResourceSlotsPerShader));
        }

        auto binding = vk::DescriptorSetLayoutBinding(bindingSlot, descriptorType, descriptorCount, shaderStages);
        if (layout->bindings.at(bindingSlot) != NullBinding)
        {
            throw nc::NcError(fmt::format("A different binding at descriptor set index {} and slot {} already exists.", setIndex, bindingSlot));
        }
        layout->bindings.at(bindingSlot) = binding;

        /* Add binding flag */
        if (layout->bindingFlags.at(bindingSlot) != vk::DescriptorBindingFlagBitsEXT())
        {
            throw nc::NcError(fmt::format("A different binding flag at descriptor set index {} and slot {} already exists.", setIndex, bindingSlot));
        }
        layout->bindingFlags.at(bindingSlot) = bindingFlags;
        layout->layout.reset();
        layout->layout = CreateDescriptorSetLayout(m_device, layout->bindings, layout->bindingFlags);
        m_perFrameSets.at(frameIndex).sets.at(setIndex) = CreateDescriptorSet(m_device, &m_pool.get(), 1, &layout->layout.get());
    }

    vk::DescriptorSetLayout* ShaderDescriptorSets::GetSetLayout(uint32_t setIndex)
    {
        NC_ASSERT(m_layouts.size() > 0, "No descriptor set layouts found. Make sure to call RegistorDescriptor to add descriptors to a set.");
        return &m_layouts.at(setIndex).layout.get();
    }

    void ShaderDescriptorSets::BindSet(uint32_t frameIndex, uint32_t setIndex, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet)
    {
        auto isDirty = m_perFrameSets.at(frameIndex).isDirty.at(setIndex);
        auto& writes = m_perFrameSets.at(frameIndex).writesPerSet.at(setIndex).writes;
        auto* set = GetSet(frameIndex, setIndex);

        /* Only update the descriptor sets if they have changed since last bind. */
        if (isDirty)
        {
            for (auto& write : writes)
            {
                write.setDstSet(*set);
            }

            m_device.updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
            m_perFrameSets.at(frameIndex).isDirty.at(setIndex) = false;
        }

        /* Bind the set to the pipeline */
        cmd->bindDescriptorSets(bindPoint, pipelineLayout, firstSet, 1, set, 0, 0);
    }

    void ShaderDescriptorSets::UpdateImage(uint32_t frameIndex, uint32_t setIndex, std::span<const vk::DescriptorImageInfo> imageInfos, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot)
    {
        vk::WriteDescriptorSet write{};
        write.setDstBinding(bindingSlot);
        write.setDstArrayElement(0);
        write.setDescriptorType(descriptorType);
        write.setDescriptorCount(descriptorCount);
        write.setPBufferInfo(0);
        write.setPImageInfo(imageInfos.data());

        auto& writes = m_perFrameSets.at(frameIndex).writesPerSet.at(setIndex).writes;

        /* Add or update the write for the image descriptor. */
        writes.at(bindingSlot) = write;
        m_perFrameSets.at(frameIndex).isDirty.at(setIndex) = true;
    }

    void ShaderDescriptorSets::UpdateBuffer(uint32_t frameIndex, uint32_t setIndex, vk::Buffer buffer, uint32_t setSize, uint32_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot)
    {
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
        write.setPBufferInfo(&bufferInfo);

        auto& writes = m_perFrameSets.at(frameIndex).writesPerSet.at(setIndex).writes;

        /* Add or update the write for the buffer descriptor. */
        writes.at(bindingSlot) = write;
        m_perFrameSets.at(frameIndex).isDirty.at(setIndex) = true;
    }
}