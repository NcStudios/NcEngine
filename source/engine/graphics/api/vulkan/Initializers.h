#pragma once

#include "Common.h"

#include "vulkan/vk_mem_alloc.hpp"

#include <span>

namespace nc::graphics
{
// Resources
auto CreateTextureSampler(vk::Device device, vk::SamplerAddressMode addressMode) -> vk::UniqueSampler;

// Pipelines
vk::PipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(ShaderStage stage, const vk::ShaderModule& shader);
vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo();
vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo(const vk::VertexInputBindingDescription& vertexInputDesc, std::span<const vk::VertexInputAttributeDescription> vertexAttributeDesc);
vk::PipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo();
vk::PipelineViewportStateCreateInfo CreateViewportCreateInfo();
vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo(vk::PolygonMode polygonMode, bool depthBiasEnable = false);
vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, bool depthBiasEnable = false);
vk::PipelineMultisampleStateCreateInfo CreateMultisampleCreateInfo(vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1);
vk::PipelineDepthStencilStateCreateInfo CreateDepthStencilCreateInfo(bool shadowMapping = false);
vk::PipelineColorBlendAttachmentState CreateColorBlendAttachmentCreateInfo(bool useAlphaBlending);
vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo();
vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment, bool useAlphaBlending);
vk::PushConstantRange CreatePushConstantRange(vk::ShaderStageFlags stageFlags, uint32_t dataTypeSize, uint32_t offset = 0u);
vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo();
vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(std::span<const vk::PushConstantRange> pushConstantRanges);
vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(std::span<const vk::DescriptorSetLayout> layouts);
vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange, std::span<const vk::DescriptorSetLayout> layouts);

// Descriptor sets
vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(vk::Device device, std::span<const vk::DescriptorSetLayoutBinding> layoutBindings, std::span<vk::DescriptorBindingFlagsEXT> bindingFlags);
vk::UniqueDescriptorSet CreateDescriptorSet(vk::Device device, vk::DescriptorPool* descriptorPool, uint32_t descriptorSetCount, vk::DescriptorSetLayout* descriptorSetLayout);
vk::DescriptorImageInfo CreateDescriptorImageInfo(vk::Sampler sampler, vk::ImageView imageView, vk::ImageLayout layout);
} // namespace nc::graphics
