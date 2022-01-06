#pragma once

#include "math/Vector.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <array>
#include <span>
#include <vector>

namespace nc::graphics
{
    class Graphics; class Base; class ITechnique;
}

namespace nc::graphics
{
    constexpr vk::SampleCountFlagBits SampleCount = vk::SampleCountFlagBits::e1;

    enum class AttachmentType : uint8_t
    {
        Color,
        Depth,
        ShadowDepth,
        Resolve
    };

    enum class ShaderStage : uint8_t
    {
        Vertex,
        Pixel
    };

    enum class ClearValue
    {
        Depth = 0,
        Color = 1,
        DepthAndColor = 2
    };

    struct AttachmentSlot
    {
        vk::AttachmentDescription description;
        vk::AttachmentReference reference;
        AttachmentType type;
    };

    struct Subpass
    {
        vk::SubpassDescription description;
        std::vector<vk::SubpassDependency> dependencies;
    };

    struct RenderTargetSize
    {
        Vector2 dimensions;
        vk::Extent2D extent;
    };

    struct RenderPass
    {
        std::string uid;
        RenderTargetSize renderTargetSize;
        vk::UniqueRenderPass renderpass;
        ClearValue valuesToClear;
        std::vector<std::unique_ptr<ITechnique>> techniques;
    };


    // Resources
    vk::SamplerCreateInfo CreateSampler(vk::SamplerAddressMode addressMode);

    // Attachments
    vk::AttachmentDescription CreateAttachmentDescription(AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp,  vk::SampleCountFlagBits numSamples);
    vk::AttachmentReference CreateAttachmentReference(AttachmentType type, uint32_t attachmentIndex);
    AttachmentSlot CreateAttachmentSlot(uint32_t attachmentIndex, AttachmentType type, vk::Format format, vk::AttachmentLoadOp loadOp, vk::AttachmentStoreOp storeOp, vk::SampleCountFlagBits numSamples);

    // Subpasses
    vk::SubpassDescription CreateSubpassDescription(const AttachmentSlot& depthAttachment);
    vk::SubpassDescription CreateSubpassDescription(const AttachmentSlot& depthAttachment, const AttachmentSlot& colorAttachment, const AttachmentSlot& resolveAttachment);
    vk::SubpassDependency CreateSubpassDependency(uint32_t sourceSubpassIndex, uint32_t destSubpassIndex, vk::PipelineStageFlags sourceStageMask, vk::PipelineStageFlags destStageMask, vk::AccessFlags sourceAccessMask,  vk::AccessFlags destAccessMask);
    vk::SubpassDependency CreateSubpassDependency(uint32_t sourceSubpassIndex, uint32_t destSubpassIndex, vk::PipelineStageFlags sourceStageMask, vk::PipelineStageFlags destStageMask, vk::AccessFlags sourceAccessMask,  vk::AccessFlags destAccessMask, vk::DependencyFlags dependencyFlags);
    Subpass CreateSubpass(const AttachmentSlot& depthAttachment);
    Subpass CreateSubpass(const AttachmentSlot& depthAttachment, const AttachmentSlot& colorAttachment, const AttachmentSlot& resolveAttachment);
    
    //Pipelines
    vk::PipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(ShaderStage stage, const vk::ShaderModule& shader);
    vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo();
    vk::PipelineVertexInputStateCreateInfo CreateVertexInputCreateInfo(const vk::VertexInputBindingDescription& vertexInputDesc, const std::array<vk::VertexInputAttributeDescription, 5>& vertexAttributeDesc);
    vk::PipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo();
    vk::PipelineViewportStateCreateInfo CreateViewportCreateInfo();
    vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo(vk::PolygonMode polygonMode, float lineWidth, bool depthBiasEnable = false);
    vk::PipelineRasterizationStateCreateInfo CreateRasterizationCreateInfo(vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, float lineWidth, bool depthBiasEnable = false);
    vk::PipelineMultisampleStateCreateInfo CreateMulitsampleCreateInfo(vk::SampleCountFlagBits numSamples = vk::SampleCountFlagBits::e1);
    vk::PipelineDepthStencilStateCreateInfo CreateDepthStencilCreateInfo(bool shadowMapping = false);
    vk::PipelineColorBlendAttachmentState CreateColorBlendAttachmentCreateInfo(bool useAlphaBlending);
    vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo();
    vk::PipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment, bool useAlphaBlending);
    vk::PushConstantRange CreatePushConstantRange(vk::ShaderStageFlags stageFlags, size_t dataTypeSize);
    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(std::span<const vk::DescriptorSetLayout> layouts);
    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange, std::span<const vk::DescriptorSetLayout> layouts);
    vk::PipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(const vk::PushConstantRange& pushConstantRange);

    // Render passes
    vk::RenderPassBeginInfo CreateRenderPassBeginInfo(vk::RenderPass& renderpass, vk::Framebuffer& framebuffer, const vk::Extent2D& extent, std::vector<vk::ClearValue>& clearValues);
    vk::UniqueRenderPass CreateRenderPass(Base* base, std::span<const AttachmentSlot> attachmentSlots, std::span<const Subpass> subpasses);

    // Screen size
    vk::Viewport CreateViewport(const Vector2& dimensions);
    vk::Extent2D CreateExtent(const Vector2& dimensions);
    vk::Rect2D CreateScissor(const vk::Extent2D& extent);
    void SetViewportAndScissor(vk::CommandBuffer* commandBuffer, const Vector2& dimensions); 

    // Descriptor sets
    vk::DescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(uint32_t binding, uint32_t descriptorCount, vk::DescriptorType type, vk::ShaderStageFlags shaderStages); 
    vk::UniqueDescriptorSetLayout CreateDescriptorSetLayout(Graphics* graphics, std::span<const vk::DescriptorSetLayoutBinding> layoutBindings, std::span<vk::DescriptorBindingFlagsEXT> bindingFlags);
    vk::UniqueDescriptorSet CreateDescriptorSet(Graphics* graphics, vk::DescriptorPool* descriptorPool, uint32_t descriptorSetCount, vk::DescriptorSetLayout* descriptorSetLayout);
    vk::WriteDescriptorSet CreateSamplerDescriptorWrite(vk::Sampler* sampler, vk::DescriptorSet* descriptorSet, uint32_t binding);
    vk::WriteDescriptorSet CreateImagesDescriptorWrite(vk::DescriptorSet* descriptorSet, std::vector<vk::DescriptorImageInfo>* imagesInfo, uint32_t imagesCount, uint32_t binding);
    vk::DescriptorImageInfo CreateDescriptorImageInfo(vk::Sampler* sampler, const vk::ImageView& imageView, vk::ImageLayout layout);

    // Clear values
    std::vector<vk::ClearValue> CreateClearValues(ClearValue value, const std::array<float, 4>& clearColor);
}
