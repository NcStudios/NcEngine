#pragma once

#include "graphics/vulkan/Mesh.h"

#include "vulkan/vulkan.hpp"
#include <array>

namespace nc::graphics::vulkan
{
    class SceneData;

    enum class AttachmentType : uint8_t
    {
        Color,
        Depth
    };

    enum class ShaderStage : uint8_t
    {
        Vertex,
        Pixel
    };

    vk::AttachmentDescription CreateAttachmentDescription(AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp);
    vk::AttachmentReference CreateAttachmentReference(AttachmentType type, uint32_t attachmentIndex);
    vk::SubpassDescription CreateSubpassDescription(const vk::AttachmentReference& colorReference, const vk::AttachmentReference& depthReference);
    vk::SubpassDependency CreateSubpassDependency(AttachmentType type);
    vk::PipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(ShaderStage stage, const vk::ShaderModule& shader);
    vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo(const vk::VertexInputBindingDescription& vertexInputDesc, const std::array<vk::VertexInputAttributeDescription, 5>& vertexAttributeDesc);
    vk::PipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo();
    vk::PipelineViewportStateCreateInfo CreateViewportCreateInfo();
    vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo();
    vk::PipelineMultisampleStateCreateInfo CreateMulitsampleCreateInfo();
    vk::PipelineDepthStencilStateCreateInfo CreateDepthStencilCreateInfo();
    vk::PipelineColorBlendAttachmentState CreateColorBlendAttachmentCreateInfo();
    vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment);
    vk::PushConstantRange CreatePushConstantRange(vk::ShaderStageFlags stageFlags, size_t dataTypeSize);
    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange, vk::DescriptorSetLayout& layout);
    vk::Viewport CreateViewport(const Vector2& dimensions);
    vk::Extent2D CreateExtent(const Vector2& dimensions);
    vk::Rect2D CreateScissor(const vk::Extent2D& extent);
    void SetViewportAndScissor(vk::CommandBuffer* commandBuffer, const Vector2& dimensions);    
}