#pragma once

#include <cstdint>

namespace nc::graphics::vulkan
{
constexpr uint32_t SampledImagesPerPoolCount = 2048u; // This is a best estimate. Can be adjusted.
constexpr uint32_t StorageBuffersPerPoolCount = 128u; // This is a best estimate. Can be adjusted.
constexpr uint32_t UniformBuffersPerPoolCount = 128u; // This is a best estimate. Can be adjusted.
constexpr uint32_t MaxSetsPerPool = 1000; // This is a best estimate. Can be adjusted.

struct DeviceRequirements
{
    DeviceRequirements(uint32_t maxBoundDescriptorSets,
                       uint32_t maxDescriptorSetUpdateAfterBindSampledImages,
                       uint32_t maxDescriptorSetUpdateAfterBindSamplers,
                       uint32_t maxDescriptorSetUpdateAfterBindStorageBuffers,
                       uint32_t maxDescriptorSetUpdateAfterBindUniformBuffers,
                       uint32_t maxPerSetDescriptors,
                       uint32_t maxPerStageUpdateAfterBindResources,
                       uint32_t maxPerStageDescriptorUpdateAfterBindSampledImages,
                       uint32_t maxPerStageDescriptorUpdateAfterBindSamplers,
                       uint32_t maxPerStageDescriptorUpdateAfterBindStorageBuffers,
                       uint32_t maxPerStageDescriptorUpdateAfterBindUniformBuffers,
                       uint32_t maxUpdateAfterBindDescriptorsInAllPools)
        : MaxBoundDescriptorSets{maxBoundDescriptorSets},
          MaxDescriptorSetUpdateAfterBindSampledImages{maxDescriptorSetUpdateAfterBindSampledImages},
          MaxDescriptorSetUpdateAfterBindSamplers{maxDescriptorSetUpdateAfterBindSamplers},
          MaxDescriptorSetUpdateAfterBindStorageBuffers{maxDescriptorSetUpdateAfterBindStorageBuffers},
          MaxDescriptorSetUpdateAfterBindUniformBuffers{maxDescriptorSetUpdateAfterBindUniformBuffers},
          MaxPerSetDescriptors{maxPerSetDescriptors},
          MaxPerStageUpdateAfterBindResources{maxPerStageUpdateAfterBindResources},
          MaxPerStageDescriptorUpdateAfterBindSampledImages{maxPerStageDescriptorUpdateAfterBindSampledImages},
          MaxPerStageDescriptorUpdateAfterBindSamplers{maxPerStageDescriptorUpdateAfterBindSamplers},
          MaxPerStageDescriptorUpdateAfterBindStorageBuffers{maxPerStageDescriptorUpdateAfterBindStorageBuffers},
          MaxPerStageDescriptorUpdateAfterBindUniformBuffers{maxPerStageDescriptorUpdateAfterBindUniformBuffers},
          MaxUpdateAfterBindDescriptorsInAllPools{maxUpdateAfterBindDescriptorsInAllPools}
    {}

    uint32_t MaxBoundDescriptorSets;
    uint32_t MaxDescriptorSetUpdateAfterBindSampledImages;
    uint32_t MaxDescriptorSetUpdateAfterBindSamplers;
    uint32_t MaxDescriptorSetUpdateAfterBindStorageBuffers;
    uint32_t MaxDescriptorSetUpdateAfterBindUniformBuffers;
    uint32_t MaxPerSetDescriptors;
    uint32_t MaxPerStageUpdateAfterBindResources;
    uint32_t MaxPerStageDescriptorUpdateAfterBindSampledImages;
    uint32_t MaxPerStageDescriptorUpdateAfterBindSamplers;
    uint32_t MaxPerStageDescriptorUpdateAfterBindStorageBuffers;
    uint32_t MaxPerStageDescriptorUpdateAfterBindUniformBuffers;
    uint32_t MaxUpdateAfterBindDescriptorsInAllPools;
};
} // namespace nc::graphics::vulkan
