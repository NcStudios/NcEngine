#pragma once

#include "graphics/vulkan/Mesh.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <array>
#include <vector>

namespace nc::graphics
{
    class Graphics2;
}

namespace nc::graphics::vulkan
{
    enum class AttachmentType : uint8_t
    {
        Color,
        Depth,
        ShadowDepth
    };

    enum class ShaderStage : uint8_t
    {
        Vertex,
        Pixel
    };

    // Resources
    vk::SamplerCreateInfo CreateSampler(vk::SamplerAddressMode addressMode);

    // Attachments
    vk::AttachmentDescription CreateAttachmentDescription(AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp);
    vk::AttachmentReference CreateAttachmentReference(AttachmentType type, uint32_t attachmentIndex);
    
    // Subpasses
    vk::SubpassDescription CreateSubpassDescription(const vk::AttachmentReference& depthReference);
    vk::SubpassDescription CreateSubpassDescription(const vk::AttachmentReference& colorReference, const vk::AttachmentReference& depthReference);
    vk::SubpassDependency CreateSubpassDependency(uint32_t sourceSubpassIndex, uint32_t destSubpassIndex, vk::PipelineStageFlags sourceStageMask, vk::PipelineStageFlags destStageMask, vk::AccessFlags sourceAccessMask,  vk::AccessFlags destAccessMask);
    vk::SubpassDependency CreateSubpassDependency(uint32_t sourceSubpassIndex, uint32_t destSubpassIndex, vk::PipelineStageFlags sourceStageMask, vk::PipelineStageFlags destStageMask, vk::AccessFlags sourceAccessMask,  vk::AccessFlags destAccessMask, vk::DependencyFlags dependencyFlags);
    
    //Pipelines
    vk::PipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(ShaderStage stage, const vk::ShaderModule& shader);
    vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo(const vk::VertexInputBindingDescription& vertexInputDesc, const std::array<vk::VertexInputAttributeDescription, 5>& vertexAttributeDesc);
    vk::PipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo();
    vk::PipelineViewportStateCreateInfo CreateViewportCreateInfo();
    vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo(vk::PolygonMode polygonMode, float lineWidth);
    vk::PipelineMultisampleStateCreateInfo CreateMulitsampleCreateInfo();
    vk::PipelineDepthStencilStateCreateInfo CreateDepthStencilCreateInfo();
    vk::PipelineColorBlendAttachmentState CreateColorBlendAttachmentCreateInfo(bool useAlphaBlending);
    vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment, bool useAlphaBlending);
    vk::PushConstantRange CreatePushConstantRange(vk::ShaderStageFlags stageFlags, size_t dataTypeSize);
    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange, const std::vector<vk::DescriptorSetLayout>& layouts);
    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange);
    
    // Screen size
    vk::Viewport CreateViewport(const Vector2& dimensions);
    vk::Extent2D CreateExtent(const Vector2& dimensions);
    vk::Rect2D CreateScissor(const vk::Extent2D& extent);
    void SetViewportAndScissor(vk::CommandBuffer* commandBuffer, const Vector2& dimensions); 

    // Descriptor sets
    vk::DescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(uint32_t binding, uint32_t descriptorCount, vk::DescriptorType type, vk::ShaderStageFlags shaderStages); 
    vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(Graphics2* graphics, std::vector<vk::DescriptorSetLayoutBinding> layoutBindings, vk::DescriptorBindingFlagsEXT bindingFlags);
    vk::UniqueDescriptorSet CreateDescriptorSet(Graphics2* graphics, vk::DescriptorPool* descriptorPool, uint32_t descriptorSetCount, vk::DescriptorSetLayout* descriptorSetLayout);
    vk::WriteDescriptorSet CreateSamplerDescriptorWrite(vk::Sampler* sampler, vk::DescriptorSet* descriptorSet, uint32_t binding);
    vk::WriteDescriptorSet CreateImagesDescriptorWrite(vk::DescriptorSet* descriptorSet, std::vector<vk::DescriptorImageInfo>* imagesInfo, uint32_t imagesCount, uint32_t binding);
    vk::DescriptorImageInfo CreateDescriptorImageInfo(vk::Sampler* sampler, const vk::ImageView& imageView, vk::ImageLayout layout);
}
