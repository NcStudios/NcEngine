#pragma once

#include "component/Component.h"

#include "vulkan/vk_mem_alloc.hpp"
#include <vector>
#include "directx/math/DirectXMath.h"

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

        struct PhongPushConstants
        {
            // N MVP matrices
            DirectX::XMMATRIX normal;
            DirectX::XMMATRIX model;
            DirectX::XMMATRIX viewProjection ;

            // Camera world position
            Vector3 cameraPos;

            // Indices into texture array
            uint32_t baseColorIndex;
            uint32_t normalColorIndex;
            uint32_t roughnessColorIndex;
        };

        class PhongAndUiTechnique
        {
            public:
                PhongAndUiTechnique(nc::graphics::Graphics2* graphics, vk::RenderPass* renderPass);
                ~PhongAndUiTechnique();
                
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
        };
    }
}