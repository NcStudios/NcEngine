#pragma once

#include "graphics/GraphicsConstants.h"
#include "utility/Signal.h"

#include "ncengine/type/StableAddress.h"

#include "vulkan/vk_mem_alloc.hpp"

#include <span>
#include <unordered_map>

namespace nc::graphics
{
using SetIndex = uint32_t;
using BindingSlot = uint32_t;

struct DescriptorSetLayout
{
    DescriptorSetLayout(bool isGlobal_)
        : isGlobal{isGlobal_}
    {
    }

    vk::UniqueDescriptorSetLayout layout;
    std::unordered_map<BindingSlot, vk::DescriptorSetLayoutBinding> bindings;
    std::unordered_map<BindingSlot,vk::DescriptorBindingFlagsEXT> bindingFlags;
    bool isGlobal;
};

struct DescriptorWrites
{
    std::unordered_map<BindingSlot, vk::WriteDescriptorSet> writes;
};

struct DescriptorSets
{
    std::unordered_map<SetIndex, vk::UniqueDescriptorSet> sets;
    std::unordered_map<SetIndex, DescriptorWrites> writesPerSet;
    std::unordered_map<SetIndex, bool> isDirty;
};

struct DescriptorSetLayoutsChanged
{
    uint32_t unused;
};

class ShaderBindingManager : StableAddress
{
    public:
        ShaderBindingManager(vk::Device device);

        /* Resources attach themselves to a shader slot by registering themselves here. */
        void RegisterDescriptor(uint32_t bindingSlot, uint32_t setIndex, size_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags, uint32_t frameIndex = std::numeric_limits<uint32_t>::max());
        void CommitResourceLayout();
        auto OnResourceLayoutChanged() -> Signal<const DescriptorSetLayoutsChanged&>& { return m_setLayoutsChanged; }

        /* Called when the data in the image or buffer changes. */
        void UpdateImage(uint32_t setIndex, std::span<const vk::DescriptorImageInfo> imageInfos, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex = std::numeric_limits<uint32_t>::max());
        void UpdateBuffer(uint32_t setIndex, vk::DescriptorBufferInfo* info, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex = std::numeric_limits<uint32_t>::max());

        /* Called in the techniques to access and bind the descriptor set(s). */
        vk::DescriptorSetLayout* GetSetLayout(uint32_t setIndex);
        void BindSet(uint32_t setIndex, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t frameIndex = std::numeric_limits<uint32_t>::max());

    private:
        auto GetSets(uint32_t frameIndex) -> DescriptorSets& { return frameIndex != std::numeric_limits<uint32_t>::max() ? m_perFrameSets.at(frameIndex) : m_globalSets; }
        auto GetSet(uint32_t frameIndex, uint32_t setIndex) -> vk::DescriptorSet* { return &GetSets(frameIndex).sets.at(setIndex).get(); }
        auto SetExists(uint32_t frameIndex, uint32_t setIndex) -> bool { return GetSets(frameIndex).sets.contains(setIndex); }
        auto LayoutExists(uint32_t setIndex) -> bool { return m_layouts.contains(setIndex); }

        vk::Device m_device;
        vk::UniqueDescriptorPool m_pool;
        std::array<DescriptorSets, MaxFramesInFlight> m_perFrameSets;
        DescriptorSets m_globalSets;
        std::unordered_map<SetIndex, DescriptorSetLayout> m_layouts;
        Signal<const DescriptorSetLayoutsChanged&> m_setLayoutsChanged;

};
}