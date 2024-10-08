#pragma once

#include "graphics/api/vulkan/NcVulkan.h"
#include "graphics/shader_resource/ShaderTypes.h"

#include <span>

namespace nc::graphics::vulkan
{
// Resources
auto CreateShadowMapSampler(vk::Device device) -> vk::UniqueSampler;
auto CreateTextureSampler(vk::Device device, vk::SamplerAddressMode addressMode) -> vk::UniqueSampler;
vk::DescriptorImageInfo CreateDescriptorImageInfo(vk::Sampler sampler, vk::ImageView imageView, vk::ImageLayout layout);

// Pipelines
vk::PipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(shader_stage stage, const vk::ShaderModule& shader);
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
} // namespace nc::graphics::vulkan
