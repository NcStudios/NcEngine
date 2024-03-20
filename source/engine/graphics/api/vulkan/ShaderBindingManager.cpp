#include "ShaderBindingManager.h"
#include "graphics/api/vulkan/Initializers.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(vk::Device device, std::span<const vk::DescriptorSetLayoutBinding> layoutBindings, std::span<vk::DescriptorBindingFlagsEXT> bindingFlags)
{
    vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo{};
    extendedInfo.setPNext(nullptr);
    extendedInfo.setBindingCount(static_cast<uint32_t>(bindingFlags.size()));
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
    renderingDescriptorPoolInfo.setMaxSets(nc::graphics::vulkan::MaxDescriptorSets);
    renderingDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(renderingPoolSizes.size()));
    renderingDescriptorPoolInfo.setPPoolSizes(renderingPoolSizes.data());

    return device.createDescriptorPoolUnique(renderingDescriptorPoolInfo);
}

auto CreatePerFrameSets(size_t initialKeyCapacity, size_t maxKeyCapacity = 65536ull)
{
    return [&] <size_t... N> (std::index_sequence<N...>)
    {
        return std::array{((void)N, nc::sparse_map<nc::graphics::vulkan::DescriptorSet>{initialKeyCapacity, maxKeyCapacity})...};
    }(std::make_index_sequence<nc::graphics::MaxFramesInFlight>());
}
}

namespace nc::graphics::vulkan
{
ShaderBindingManager::ShaderBindingManager(vk::Device device)
    : m_device{device},
      m_pool{CreateRenderingDescriptorPool(device)},
      m_perFrameSets{CreatePerFrameSets(MaxSetsDivided, MaxSetsDivided)},
      m_staticSets{MaxSetsDivided, MaxSetsDivided},
      m_layouts{vulkan::MaxDescriptorSets, vulkan::MaxDescriptorSets}
{
}

void ShaderBindingManager::RegisterDescriptor(uint32_t bindingSlot, uint32_t setIndex, size_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags, uint32_t frameIndex)
{
    NC_ASSERT(bindingSlot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");

    if (!m_layouts.contains(setIndex))
        m_layouts.emplace(setIndex, DescriptorSetLayout(frameIndex == StaticSet));

    auto& layout = m_layouts.at(setIndex);
    layout.bindings.erase(bindingSlot);
    layout.bindings.emplace(bindingSlot, vk::DescriptorSetLayoutBinding(bindingSlot, descriptorType, static_cast<uint32_t>(descriptorCount), shaderStages));
    layout.bindingFlags.erase(bindingSlot);
    layout.bindingFlags.emplace(bindingSlot, vk::DescriptorBindingFlagsEXT(bindingFlags));
    
    layout.layout = CreateDescriptorSetLayout(m_device, layout.bindings.values(), layout.bindingFlags.values());

    auto& sets = GetSets(frameIndex);
    if (!sets.contains(setIndex))
    {
        sets.emplace(setIndex, DescriptorSet{});
    }
}

void ShaderBindingManager::CommitResourceLayout()
{
    for (auto [setIndex, layout] : std::views::zip(m_layouts.keys(), m_layouts.values()))
    {
        if (layout.isStatic)
        {
            auto& set = GetSets(StaticSet).at(setIndex);
            set.set = CreateDescriptorSet(m_device, &m_pool.get(), 1, &layout.layout.get());
            continue;
        }

        for (auto& sets : m_perFrameSets)
        {
            auto& set = sets.at(setIndex);
            set.set = CreateDescriptorSet(m_device, &m_pool.get(), 1, &layout.layout.get());
        }
    }

    m_setLayoutsChanged.Emit(DescriptorSetLayoutsChanged{StaticSet});
}

vk::DescriptorSetLayout* ShaderBindingManager::GetSetLayout(uint32_t setIndex)
{
    NC_ASSERT(m_layouts.contains(setIndex), fmt::format("No descriptor set layout at index: {} found. Make sure to call RegistorDescriptor to add descriptors to a set.", setIndex));
    return &m_layouts.at(setIndex).layout.get();
}

void ShaderBindingManager::BindSet(uint32_t setIndex, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t , uint32_t frameIndex)
{
    auto& set = GetSets(frameIndex).at(setIndex);
    auto& writes = set.writes;

    /* Only update the descriptor sets if they have changed since last bind. */
    if (set.isDirty)
    {
        for (auto& write : writes.values())
        {
            write.setDstSet(*set.set);
        }

        m_device.updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.values().data(), 0, nullptr);
        set.isDirty = false;
    }

    /* Bind the set to the pipeline */
    cmd->bindDescriptorSets(bindPoint, pipelineLayout, setIndex, 1, &set.set.get(), 0, 0);
}

void ShaderBindingManager::UpdateImage(uint32_t setIndex, std::span<const vk::DescriptorImageInfo> imageInfos, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex)
{
    vk::WriteDescriptorSet write{};
    write.setDstBinding(bindingSlot);
    write.setDstArrayElement(0);
    write.setDescriptorType(descriptorType);
    write.setDescriptorCount(static_cast<uint32_t>(descriptorCount));
    write.setPBufferInfo(0);
    write.setPImageInfo(imageInfos.data());

    auto& set = GetSets(frameIndex).at(setIndex);
    auto& writes = set.writes;

    /* Add or update the write for the buffer descriptor. */
    writes.erase(bindingSlot);
    writes.emplace(bindingSlot, write);

    set.isDirty = true;
}

void ShaderBindingManager::UpdateBuffer(uint32_t setIndex, vk::DescriptorBufferInfo* info, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex)
{
    vk::WriteDescriptorSet write{};
    write.setDstBinding(bindingSlot);
    write.setDstArrayElement(0);
    write.setDescriptorType(descriptorType);
    write.setDescriptorCount(static_cast<uint32_t>(descriptorCount));
    write.setPImageInfo(0);
    write.setPBufferInfo(info);

    auto& set = GetSets(frameIndex).at(setIndex);
    auto& writes = set.writes;

    /* Add or update the write for the buffer descriptor. */
    writes.erase(bindingSlot);
    writes.emplace(bindingSlot, write);

    set.isDirty = true;
}
} // namespace nc::graphics::vulkan
