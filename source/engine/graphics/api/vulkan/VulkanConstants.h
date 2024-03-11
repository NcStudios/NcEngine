#pragma once

#include <cstdint>

namespace nc::graphics::vulkan
{
/**
 * @brief Credit goes to vulkan.gpuinfo.org
 * Information gathered March 2024
 */

/**-----------------------------------
 Feature Support - Desktop Platforms
 -------------------------------------
| Feature | Windows|Linux |
|----------------------------------------------|--------|-------|
| fillModeNonSolid |99.76% |99.72% |
| samplerAnisotropy |99.18% |99.72% |
| sampleRateShading |99.06% |99.72% |
| descriptorBindingPartiallyBound |94.44% |91.11% |
| descriptorBindingSampledImageUpdateAfterBind |96.82% |91.11% |
| descriptorBindingStorageBufferUpdateAfterBind|96.82% |91.11% |
| descriptorBindingUniformBufferUpdateAfterBind|70.27% |72.82% |
| descriptorIndexing |96.82% |90.10% |
| runtimeDescriptorArray |96.82% |90.10% |
| shaderSampledImageArrayNonUniformIndexing |96.82% |90.10% |
| shaderStorageBufferArrayNonUniformIndexing |96.82% |90.94% |
| shaderUniformBufferArrayNonUniformIndexing |96.18% |89.43% |
*/

/** How many total descriptor sets can be created; */
constexpr uint32_t MaxDescriptorSets = 10u;

/** maxBoundDescriptorSets is the maximum number of descriptor sets that can be simultaneously used by a pipeline.
 * All DescriptorSet decorations in shader modules must have a value less than maxBoundDescriptorSets.
 * Total Adoption: 96.9% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxBoundDescriptorSets = 8u;

/** maxDescriptorSetSampledImages is the maximum number of sampled images that can be included in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, or VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * Total Adoption: 99.8% Windows
 * Total Adoption: 99.1% Linux */
constexpr uint32_t MaxDescriptorSetSampledImages = 256u;

/** maxDescriptorSetSamplers is the maximum number of samplers that can be included in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_SAMPLER or VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * Total Adoption: 97.6% Windows
 * Total Adoption: 94.8% Linux */
constexpr uint32_t MaxDescriptorSetSamplers = 576u;

/** maxDescriptorSetStorageBuffers is the maximum number of storage buffers that can be included in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_STORAGE_BUFFER or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * Total Adoption: 99.8% Windows
 * Total Adoption: 99.7% Linux */
constexpr uint32_t MaxDescriptorSetStorageBuffers = 96u;

/** maxDescriptorSetUniformBuffers is the maximum number of uniform buffers that can be included in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 100.0% Linux*/
constexpr uint32_t MaxDescriptorSetUniformBuffers = 72u;

/** maxDescriptorSetUpdateAfterBindSampledImages is similar to maxDescriptorSetSampledImages but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxDescriptorSetUpdateAfterBindSampledImages = 65536u;

/** maxDescriptorSetUpdateAfterBindSamplers is similar to maxDescriptorSetSamplers but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxDescriptorSetUpdateAfterBindSamplers = 65536u;

/** maxDescriptorSetUpdateAfterBindStorageBuffers is similar to maxDescriptorSetStorageBuffers but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxDescriptorSetUpdateAfterBindStorageBuffers = 65536u;

/** maxDescriptorSetUpdateAfterBindUniformBuffers is similar to maxDescriptorSetUniformBuffers but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.7% Linux */
constexpr uint32_t MaxDescriptorSetUpdateAfterBindUniformBuffers = 90u;

/** maxPerSetDescriptors is a maximum number of descriptors (summed over all descriptor types) in a single descriptor set that is guaranteed to satisfy any implementation-dependent constraints on the size of a descriptor set itself.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 100.0% Linux */
constexpr uint32_t MaxPerSetDescriptors = 1024u;

/** maxPerStageDescriptorSampledImages is the maximum number of sampled images that can be accessible to a single shader stage in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, or VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * A descriptor is accessible to a pipeline shader stage when the stageFlags member of the VkDescriptorSetLayoutBinding structure has the bit for that shader stage set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 97.0% Linux */
constexpr uint32_t MaxPerStageDescriptorSampledImages = 128u;

/** maxPerStageDescriptorSamplers is the maximum number of samplers that can be accessible to a single shader stage in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_SAMPLER or VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * A descriptor is accessible to a shader stage when the stageFlags member of the VkDescriptorSetLayoutBinding structure has the bit for that shader stage set.
 * Total Adoption: 99.3% Windows
 * Total Adoption: 97.7% Linux */
constexpr uint32_t MaxPerStageDescriptorSamplers = 32u;

/** maxPerStageDescriptorStorageBuffers is the maximum number of storage buffers that can be accessible to a single shader stage in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_STORAGE_BUFFER or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * A descriptor is accessible to a pipeline shader stage when the stageFlags member of the VkDescriptorSetLayoutBinding structure has the bit for that shader stage set.
 * Total Adoption: 99.9% Windows
 * Total Adoption: 99.8% Linux */
constexpr uint32_t MaxPerStageDescriptorStorageBuffers = 16u;

/** maxPerStageDescriptorUniformBuffers is the maximum number of uniform buffers that can be accessible to a single shader stage in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC count against this limit.
 * Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * A descriptor is accessible to a shader stage when the stageFlags member of the VkDescriptorSetLayoutBinding structure has the bit for that shader stage set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 100.0% Linux */
constexpr uint32_t MaxPerStageDescriptorUniformBuffers = 12u;

/** maxPerStageDescriptorUpdateAfterBindSampledImages is similar to maxPerStageDescriptorSampledImages
 * but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxPerStageDescriptorUpdateAfterBindSampledImages = 65536u;

/** maxPerStageDescriptorUpdateAfterBindSamplers is similar to maxPerStageDescriptorSamplers
 * but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxPerStageDescriptorUpdateAfterBindSamplers = 65536u;

/** maxPerStageDescriptorUpdateAfterBindStorageBuffers is similar to maxPerStageDescriptorStorageBuffers
 * but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxPerStageDescriptorUpdateAfterBindStorageBuffers = 65536u;

/** maxPerStageDescriptorUpdateAfterBindUniformBuffers is similar to maxPerStageDescriptorUniformBuffers
 * but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.7% Linux */
constexpr uint32_t MaxPerStageDescriptorUpdateAfterBindUniformBuffers = 15u;

/** maxPerStageUpdateAfterBindResources is similar to maxPerStageResources
 * but counts descriptors from descriptor sets created with or without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 99.4% Linux */
constexpr uint32_t MaxPerStageUpdateAfterBindResources = 65536u;

/** maxPerStageResources is the maximum number of resources that can be accessible to a single shader stage in a pipeline layout.
 * Descriptors with a type of VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
 * VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
 * or VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT count against this limit. Only descriptors in descriptor set layouts created without the VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT bit set count against this limit.
 * For the fragment shader stage the framebuffer color attachments also count against this limit.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 100.0% Linux */
constexpr uint32_t MaxPerStageResources = 128u;

/** maxPushConstantsSize is the maximum size, in bytes, of the pool of push constant memory.
 * For each of the push constant ranges indicated by the pPushConstantRanges member of the VkPipelineLayoutCreateInfo structure, (offset + size) must be less than or equal to this limit.
 * Total Adoption: 100.0% Windows
 * Total Adoption: 100.0% Linux */
constexpr uint32_t MaxPushConstantsSize = 128u;

/** maxVertexInputBindings is the maximum number of vertex buffers that can be specified for providing vertex attributes to a graphics pipeline.
 * These are described in the array of VkVertexInputBindingDescription structures that are provided at graphics pipeline creation time via the pVertexBindingDescriptions member of the VkPipelineVertexInputStateCreateInfo structure.
 * The binding member of VkVertexInputBindingDescription must be less than this limit.
 * Total Adoption: 95.8% Windows
 * Total Adoption: 99.1% Linux */
constexpr uint32_t MaxVertexInputBindings = 28u;

/** maxUpdateAfterBindDescriptorsInAllPools is the maximum number of descriptors (summed over all descriptor types) that can be created across all pools that are created with the VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT bit set.
 * Pool creation may fail when this limit is exceeded, or when the space this limit represents is unable to satisfy a pool creation due to fragmentation.
 * Total Adoption: 97.9% Windows
 * Total Adoption: 99.0% Linux */
constexpr uint32_t MaxUpdateAfterBindDescriptorsInAllPools = 1000000u;

} // namespace nc::graphics::vulkan
