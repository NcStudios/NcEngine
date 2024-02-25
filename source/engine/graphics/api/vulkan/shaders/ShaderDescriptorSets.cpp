#include "ShaderDescriptorSets.h"
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
    renderingDescriptorPoolInfo.setMaxSets(10);
    renderingDescriptorPoolInfo.setPoolSizeCount(static_cast<uint32_t>(renderingPoolSizes.size()));
    renderingDescriptorPoolInfo.setPPoolSizes(renderingPoolSizes.data());

    return device.createDescriptorPoolUnique(renderingDescriptorPoolInfo);
}

template<typename T> 
auto AddOrUpdate(uint32_t bindingSlot, std::unordered_map<uint32_t, T>& collection, T itemToAdd) -> std::vector<T>
{
    if (!collection.contains(bindingSlot))
    {
        collection.emplace(bindingSlot, itemToAdd);
    }
    else
    {
        collection.at(bindingSlot) = itemToAdd;
    }

    auto flattenedCollection = std::vector<T>{};
    flattenedCollection.reserve(collection.size());
    std::ranges::transform(collection, std::back_inserter(flattenedCollection), [](auto&& kvp)
    {
        return kvp.second;
    });
    return flattenedCollection;
}
}

namespace nc::graphics
{
ShaderDescriptorSets::ShaderDescriptorSets(vk::Device device)
    : m_device{device},
        m_pool{CreateRenderingDescriptorPool(device)},
        m_perFrameSets{},
        m_globalSets{},
        m_layouts{}
{
}

void ShaderDescriptorSets::RegisterDescriptor(uint32_t bindingSlot, uint32_t setIndex, size_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags, uint32_t frameIndex)
{
    NC_ASSERT(bindingSlot < MaxResourceSlotsPerShader, "Binding slot exceeds the maximum allowed resource bindings.");

    if (!LayoutExists(setIndex))
    {
        m_layouts.emplace(setIndex, DescriptorSetLayout(frameIndex == std::numeric_limits<uint32_t>::max()));
    }

    auto* layout = &m_layouts.at(setIndex);

    auto flattenedBindings = AddOrUpdate(bindingSlot, layout->bindings, vk::DescriptorSetLayoutBinding(bindingSlot, descriptorType, static_cast<uint32_t>(descriptorCount), shaderStages));
    auto flattenedFlags = AddOrUpdate(bindingSlot, layout->bindingFlags, vk::DescriptorBindingFlagsEXT(bindingFlags));

    layout->layout.reset();
    layout->layout = CreateDescriptorSetLayout(m_device, flattenedBindings, flattenedFlags);
}

void ShaderDescriptorSets::CommitResourceLayout()
{
    for (auto& [setIndex, layout] : m_layouts)
    {
        if (layout.isGlobal)
        {
            auto& sets = GetSets(std::numeric_limits<uint32_t>::max()).sets;
            if (!sets.contains(setIndex))
            {
                sets.emplace(setIndex, CreateDescriptorSet(m_device, &m_pool.get(), 1, &layout.layout.get()));
            }
            else
            {
                sets.at(setIndex) = CreateDescriptorSet(m_device, &m_pool.get(), 1, &layout.layout.get());
            }
        }
        else
        {
            for (auto i : std::views::iota(0u, MaxFramesInFlight))
            {
                auto& sets = GetSets(i).sets;
                if (!sets.contains(setIndex))
                {
                    sets.emplace(setIndex, CreateDescriptorSet(m_device, &m_pool.get(), 1, &layout.layout.get()));
                }
                else
                {
                    sets.at(setIndex) = CreateDescriptorSet(m_device, &m_pool.get(), 1, &layout.layout.get());
                }
            }
        }
    }

    m_setLayoutsChanged.Emit(DescriptorSetLayoutsChanged{});
}

vk::DescriptorSetLayout* ShaderDescriptorSets::GetSetLayout(uint32_t setIndex)
{
    NC_ASSERT(m_layouts.contains(setIndex), fmt::format("No descriptor set layout at index: {} found. Make sure to call RegistorDescriptor to add descriptors to a set.", setIndex));
    return &m_layouts.at(setIndex).layout.get();
}

void ShaderDescriptorSets::BindSet(uint32_t setIndex, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t , uint32_t frameIndex)
{
    auto& sets = GetSets(frameIndex);
    auto& isDirty = sets.isDirty.at(setIndex);
    auto& writes = sets.writesPerSet.at(setIndex).writes;
    auto* set = GetSet(frameIndex, setIndex);

    /* Only update the descriptor sets if they have changed since last bind. */
    if (isDirty)
    {
        auto flattenedWrites = std::vector<vk::WriteDescriptorSet>{};
        flattenedWrites.reserve(writes.size());
        std::ranges::transform(writes, std::back_inserter(flattenedWrites), [set](auto &&write)
        {
            write.second.setDstSet(*set);
            return write.second;
        });

        m_device.updateDescriptorSets(static_cast<uint32_t>(flattenedWrites.size()), flattenedWrites.data(), 0, nullptr);
        isDirty = false;
    }

    /* Bind the set to the pipeline */
    cmd->bindDescriptorSets(bindPoint, pipelineLayout, setIndex, 1, set, 0, 0);
}

void ShaderDescriptorSets::UpdateImage(uint32_t setIndex, std::span<const vk::DescriptorImageInfo> imageInfos, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex)
{
    vk::WriteDescriptorSet write{};
    write.setDstBinding(bindingSlot);
    write.setDstArrayElement(0);
    write.setDescriptorType(descriptorType);
    write.setDescriptorCount(static_cast<uint32_t>(descriptorCount));
    write.setPBufferInfo(0);
    write.setPImageInfo(imageInfos.data());
    
    auto& sets = GetSets(frameIndex);

    auto& writesPerSet = sets.writesPerSet;
    auto& isDirty = sets.isDirty;

    if (!writesPerSet.contains(setIndex))
        writesPerSet.emplace(setIndex, DescriptorWrites{});

    auto& writes = writesPerSet.at(setIndex).writes;

    /* Add or update the write for the buffer descriptor. */
    if (!writes.contains(bindingSlot))
        writes.emplace(bindingSlot, write);
    else
        writes.at(bindingSlot) = write;

    if (!isDirty.contains(setIndex))
        isDirty.emplace(setIndex, true);
    else
        isDirty.at(setIndex) = true;
}

void ShaderDescriptorSets::UpdateBuffer(uint32_t setIndex, vk::DescriptorBufferInfo* info, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex)
{
    vk::WriteDescriptorSet write{};
    write.setDstBinding(bindingSlot);
    write.setDstArrayElement(0);
    write.setDescriptorType(descriptorType);
    write.setDescriptorCount(static_cast<uint32_t>(descriptorCount));
    write.setPImageInfo(0);
    write.setPBufferInfo(info);

    auto& sets = GetSets(frameIndex);

    auto& writesPerSet = sets.writesPerSet;
    auto& isDirty = sets.isDirty;

    if (!writesPerSet.contains(setIndex))
        writesPerSet.emplace(setIndex, DescriptorWrites{});

    auto& writes = writesPerSet.at(setIndex).writes;

    /* Add or update the write for the buffer descriptor. */
    if (!writes.contains(bindingSlot))
        writes.emplace(bindingSlot, write);
    else
        writes.at(bindingSlot) = write;

    if (!isDirty.contains(setIndex))
        isDirty.emplace(setIndex, true);
    else
        isDirty.at(setIndex) = true;
}
}