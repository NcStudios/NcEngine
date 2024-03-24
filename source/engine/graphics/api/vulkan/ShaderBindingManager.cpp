#include "ShaderBindingManager.h"
#include "graphics/api/vulkan/core/DeviceRequirements.h"
#include "graphics/api/vulkan/Initializers.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
auto CreateDescriptorSetLayout(vk::Device device, std::span<const vk::DescriptorSetLayoutBinding> layoutBindings, std::span<vk::DescriptorBindingFlagsEXT> bindingFlags) -> vk::UniqueDescriptorSetLayout
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

auto CreateDescriptorSet(vk::Device device, vk::DescriptorPool* descriptorPool, uint32_t descriptorSetCount, vk::DescriptorSetLayout* descriptorSetLayout) -> vk::UniqueDescriptorSet
{
    vk::DescriptorSetAllocateInfo allocationInfo{};
    allocationInfo.setPNext(nullptr);
    allocationInfo.setDescriptorPool(*descriptorPool);
    allocationInfo.setDescriptorSetCount(descriptorSetCount);
    allocationInfo.setPSetLayouts(descriptorSetLayout);

    // @todo: return the vector rather than the indiviual item, don't use move in return values
    return std::move(device.allocateDescriptorSetsUnique(allocationInfo)[0]);
}

auto CreateDescriptorPool(vk::Device device, vk::DescriptorPoolCreateFlags flags) -> vk::UniqueDescriptorPool
{
    static constexpr auto renderingPoolSizes = std::array
    {
        vk::DescriptorPoolSize { vk::DescriptorType::eCombinedImageSampler, nc::graphics::vulkan::SampledImagesPerPoolCount },
        vk::DescriptorPoolSize { vk::DescriptorType::eStorageBuffer,  nc::graphics::vulkan::StorageBuffersPerPoolCount },
        vk::DescriptorPoolSize { vk::DescriptorType::eUniformBuffer,  nc::graphics::vulkan::UniformBuffersPerPoolCount }
    };

    auto renderingDescriptorPoolInfo = vk::DescriptorPoolCreateInfo(
        flags,
        nc::graphics::vulkan::MaxSetsPerPool,
        static_cast<uint32_t>(renderingPoolSizes.size()),
        renderingPoolSizes.data()
    );

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
DescriptorPool::DescriptorPool(vk::Device device, vk::DescriptorPoolCreateFlags flags, uint32_t sampledImagesCount, uint32_t storageBuffersCount, uint32_t uniformBuffersCount)
    : pool{CreateDescriptorPool(device, flags)},
      currentSampledImagesCount{currentSampledImagesCount + sampledImagesCount},
      currentStorageBuffersCount{currentStorageBuffersCount + storageBuffersCount},
      currentUniformBuffersCount{currentUniformBuffersCount + uniformBuffersCount}
{}

DescriptorAllocator::DescriptorAllocator(vk::Device device, const DeviceRequirements* deviceRequirements)
    : m_device{device},
      m_deviceRequirements{deviceRequirements},
      m_totalSampledImagesCount{0u},
      m_totalStorageBuffersCount{0u},
      m_totalUniformBuffersCount{0u}
{
}

auto DescriptorAllocator::GetFreePool(vk::DescriptorPoolCreateFlags flags, uint32_t sampledImagesCount, uint32_t storageBuffersCount, uint32_t uniformBuffersCount) -> DescriptorPool&
{
    auto pos = std::find(m_pools.begin(), m_pools.end(), [sampledImagesCount, storageBuffersCount, uniformBuffersCount](auto&& pool)
    {
        return pool.currentSampledImagesCount + sampledImagesCount <= SampledImagesPerPoolCount &&
               pool.currentStorageBuffersCount + storageBuffersCount <= StorageBuffersPerPoolCount &&
               pool.currentUniformBuffersCount + uniformBuffersCount <= UniformBuffersPerPoolCount;
    });

    if (pos == m_pools.end() || pos->setsCount >= MaxSetsPerPool)
    {
        m_pools.emplace_back(m_device, flags);
    }

    return m_pools.back();
}

/**
 * Create the initial pool.
 * Track the count of each descriptor type remaining in the pool.
 * When allocating a descriptor set, validate the counts do not exceed:
 *  A) Each descriptor type count does not exceed the pool's remaining counts per type
 *  B) The descriptor set's counts per type plus all existing allocated counts per type do not exceed the hardware maximums
 *  C) The descriptor set's counts plus all existing allocated counts do not exceed MaxUpdateAfterBindDescriptorsInAllPools
 *  D) The descriptor type count does not exceed the pool's total counts per type
 *  E) The count of descriptor sets plus the existing sets in the pool does not exceed the MaxSets.
 *  F) The count of descriptors in the set does not exceed MaxPerSetDescriptors
 * If A, B, C, D, E and F are true, allocate the descriptor set in the pool.
 * If B is or C is or D or F is false, throw an exception.
 * If only A and/or E is false, create a new descriptor pool and allocate from it.
 * Increment the counts.
 * */
auto DescriptorAllocator::Allocate(DescriptorSetLayout* layout, vk::DescriptorPoolCreateFlags flags) ->  vk::UniqueDescriptorSet
{
    auto sampledImagesCount = 0u;
    auto storageBuffersCount = 0u;
    auto uniformBuffersCount = 0u;

    for (auto& binding : layout->bindings.values())
    {
        switch (binding.descriptorType)
        {
            case vk::DescriptorType::eSampledImage:
            {
                sampledImagesCount += binding.descriptorCount;
                break;
            }
            case vk::DescriptorType::eStorageBuffer:
            {
                storageBuffersCount += binding.descriptorCount;
                break;
            }
            case vk::DescriptorType::eUniformBuffer:
            {
                uniformBuffersCount += binding.descriptorCount;
                break;
            }
            default:
            {
                throw NcError("Descriptor type not supported.");
            }
        }
    }

    NC_ASSERT(sampledImagesCount <= SampledImagesPerPoolCount, fmt::format("Cannot allocate a descriptor set with {} sampled images. The limit per pool is {}. Consider splitting sampled images into multiple descriptor sets.", sampledImagesCount, SampledImagesPerPoolCount));
    NC_ASSERT(storageBuffersCount <= StorageBuffersPerPoolCount, fmt::format("Cannot allocate a descriptor set with {} storage buffers. The limit per pool is {}. Consider splitting storage buffers into multiple descriptor sets.", storageBuffersCount, StorageBuffersPerPoolCount));
    NC_ASSERT(uniformBuffersCount <= UniformBuffersPerPoolCount, fmt::format("Cannot allocate a descriptor set with {} uniform buffers. The limit per pool is {}. Consider splitting uniform buffers into multiple descriptor sets.", uniformBuffersCount, UniformBuffersPerPoolCount));

    NC_ASSERT(sampledImagesCount <= m_deviceRequirements->MaxDescriptorSetUpdateAfterBindSampledImages, fmt::format("Cannot allocate a descriptor set with {} sampled images. This descriptor set could never be bound as the maximum count of sampled images bound per stage is: {}", sampledImagesCount, m_deviceRequirements->MaxPerStageDescriptorUpdateAfterBindSampledImages));
    NC_ASSERT(storageBuffersCount <= m_deviceRequirements->MaxDescriptorSetUpdateAfterBindStorageBuffers, fmt::format("Cannot allocate a descriptor set with {} storage buffers. This descriptor set could never be bound as the maximum count of storage buffers bound per stage is: {}", storageBuffersCount, m_deviceRequirements->MaxPerStageDescriptorUpdateAfterBindStorageBuffers));
    NC_ASSERT(uniformBuffersCount <= m_deviceRequirements->MaxDescriptorSetUpdateAfterBindUniformBuffers, fmt::format("Cannot allocate a descriptor set with {} uniform buffers. This descriptor set could never be bound as the maximum count of uniform buffers bound per stage is: {}", uniformBuffersCount, m_deviceRequirements->MaxPerStageDescriptorUpdateAfterBindUniformBuffers));

    NC_ASSERT(sampledImagesCount + storageBuffersCount + uniformBuffersCount <= m_deviceRequirements->MaxPerSetDescriptors, fmt::format("Cannot allocate a descriptor set with {} descriptors. The maximum per MaxPerSetDescriptors is: {}.", sampledImagesCount + storageBuffersCount + uniformBuffersCount, m_deviceRequirements->MaxPerSetDescriptors));

    auto newSampledImagesTotal = sampledImagesCount + m_totalSampledImagesCount;
    auto newStorageBuffersTotal = storageBuffersCount + m_totalStorageBuffersCount;
    auto newUniformBuffersTotal = uniformBuffersCount + m_totalUniformBuffersCount;
    NC_ASSERT(newSampledImagesTotal + newStorageBuffersTotal + newUniformBuffersTotal <= m_deviceRequirements->MaxUpdateAfterBindDescriptorsInAllPools, fmt::format("This descriptor set cannot be allocated as the total count of descriptors {} would exceed the total allowed descriptors across all pools: {}.", newSampledImagesTotal + newStorageBuffersTotal + newUniformBuffersTotal, m_deviceRequirements->MaxUpdateAfterBindDescriptorsInAllPools));

    m_totalSampledImagesCount = newSampledImagesTotal;
    m_totalStorageBuffersCount = newStorageBuffersTotal;
    m_totalUniformBuffersCount = newUniformBuffersTotal;

    auto& pool = GetFreePool(flags, sampledImagesCount, storageBuffersCount, uniformBuffersCount);

    return CreateDescriptorSet(m_device, &pool.pool.get(), 1, &layout->layout.get());
}

ShaderBindingManager::ShaderBindingManager(vk::Device device, const DeviceRequirements* deviceRequirements)
    : m_device{device},
      m_deviceRequirements{deviceRequirements},
      m_allocator{DescriptorAllocator(device, m_deviceRequirements)},
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
        auto poolFlags = layout.bindingFlags & vk::DescriptorBindingFlagBits::eUpdateAfterBind != vk::DescriptorBindingFlags() ? vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind : vk::DescriptorPoolCreateFlags();
        if (layout.isStatic)
        {
            auto& set = GetSets(StaticSet).at(setIndex);
            layout.layout = CreateDescriptorSetLayout(m_device, layout.bindings.values(), layout.bindingFlags.values());
            set.set = m_pools.Allocate(&layout->layout.get(), poolFlags, )
            continue;
        }

        for (auto& sets : m_perFrameSets)
        {
            auto& set = sets.at(setIndex);
            layout.layout = CreateDescriptorSetLayout(m_device, layout.bindings.values(), layout.bindingFlags.values());
            set.set = m_pools.Allocate(&layout->layout.get(), poolFlags, )
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
