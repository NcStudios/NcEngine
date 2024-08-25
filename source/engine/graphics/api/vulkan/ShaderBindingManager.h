#pragma once

#include "graphics/api/vulkan/NcVulkan.h"
#include "graphics/api/vulkan/VulkanConstants.h"
#include "graphics/GraphicsConstants.h"
#include "utility/Signal.h"

#include "ncengine/type/StableAddress.h"
#include "ncengine/utility/SparseMap.h"

#include <span>
#include <unordered_map>

namespace nc::graphics::vulkan
{
constexpr auto MaxSetsDivided = vulkan::MaxDescriptorSets/(MaxFramesInFlight+1);
constexpr auto StaticSet = std::numeric_limits<uint32_t>::max();

struct DescriptorSetLayout
{
    DescriptorSetLayout(bool isStatic_)
        : layout{nullptr},
          bindings{DefaultResourceSlotsPerShader, MaxResourceSlotsPerShader},
          bindingFlags{DefaultResourceSlotsPerShader, MaxResourceSlotsPerShader},
          isStatic{isStatic_}
    {
    }
    vk::UniqueDescriptorSetLayout layout;
    nc::sparse_map<vk::DescriptorSetLayoutBinding> bindings;
    nc::sparse_map<vk::DescriptorBindingFlagsEXT> bindingFlags;
    bool isStatic;
};

struct DescriptorSet
{
    vk::UniqueDescriptorSet set;
    nc::sparse_map<vk::WriteDescriptorSet> writes{DefaultResourceSlotsPerShader, MaxResourceSlotsPerShader};
    bool isDirty;
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
        void RegisterDescriptor(uint32_t bindingSlot, uint32_t setIndex, size_t descriptorCount, vk::DescriptorType descriptorType, vk::ShaderStageFlags shaderStages, vk::DescriptorBindingFlagBitsEXT bindingFlags, uint32_t frameIndex = StaticSet);
        void CommitResourceLayout();
        auto OnResourceLayoutChanged() -> Signal<const DescriptorSetLayoutsChanged&>& { return m_setLayoutsChanged; }

        /* Called when the data in the image or buffer changes. */
        void UpdateImage(uint32_t setIndex, std::span<const vk::DescriptorImageInfo> imageInfos, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex = StaticSet);
        void UpdateBuffer(uint32_t setIndex, vk::DescriptorBufferInfo* info, size_t descriptorCount, vk::DescriptorType descriptorType, uint32_t bindingSlot, uint32_t frameIndex = StaticSet);

        /* Called in the techniques to access and bind the descriptor set(s). */
        vk::DescriptorSetLayout* GetSetLayout(uint32_t setIndex);
        void BindSet(uint32_t setIndex, vk::CommandBuffer* cmd, vk::PipelineBindPoint bindPoint, vk::PipelineLayout pipelineLayout, uint32_t firstSet, uint32_t frameIndex = StaticSet);

    private:
        auto GetSets(uint32_t frameIndex) -> nc::sparse_map<DescriptorSet>& { return frameIndex == StaticSet ? m_staticSets : m_perFrameSets.at(frameIndex); }
        auto GetDescriptorSet(uint32_t frameIndex, uint32_t setIndex) -> vk::DescriptorSet* { return &GetSets(frameIndex).at(setIndex).set.get(); }

        vk::Device m_device;
        vk::UniqueDescriptorPool m_pool;
        std::array<nc::sparse_map<DescriptorSet>, MaxFramesInFlight> m_perFrameSets;
        nc::sparse_map<DescriptorSet> m_staticSets;
        nc::sparse_map<DescriptorSetLayout> m_layouts;
        Signal<const DescriptorSetLayoutsChanged&> m_setLayoutsChanged;

};
} // namespace nc::graphics::vulkan
