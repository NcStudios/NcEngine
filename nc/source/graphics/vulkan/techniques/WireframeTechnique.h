#pragma once

#include "component/Component.h"

#include "vulkan/vk_mem_alloc.hpp"
#include "directx/math/DirectXMath.h"
#include <vector>

namespace nc::vulkan
{
    class MeshRenderer;
}

namespace nc::graphics
{
    class Graphics2;

    namespace vulkan
    {
        class Commands; class Base; class Swapchain;

        struct WireframePushConstants
        {
            // N MVP matrices
            DirectX::XMMATRIX normal;
            DirectX::XMMATRIX model;
            DirectX::XMMATRIX viewProjection;
        };

        class WireframeTechnique
        {
            public:
                WireframeTechnique(nc::graphics::Graphics2* graphics, vk::RenderPass* renderPass);
                ~WireframeTechnique();

                void Bind(vk::CommandBuffer* cmd);
                std::vector<Entity>* RegisterMeshRenderer(nc::vulkan::MeshRenderer* meshRenderer);
                void Record(vk::CommandBuffer* cmd);
                void Clear();

            private:
                void CreatePipeline(vk::RenderPass* renderPass);

                std::unordered_map<std::string, std::vector<Entity>> m_meshRenderers;

                nc::graphics::Graphics2* m_graphics;
                Base* m_base;
                Swapchain* m_swapchain;
                vk::Pipeline m_pipeline;
                vk::PipelineLayout m_pipelineLayout;
                vk::DescriptorSetLayout m_descriptorSetLayout;
        };
    }
}