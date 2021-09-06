#pragma once

#include "component/Component.h"
#include "vulkan/vk_mem_alloc.hpp"
#include "directx/math/DirectXMath.h"

#include <vector>
#include <span>

namespace nc
{
    class MeshRenderer;
}

namespace nc::graphics
{
    class Graphics2; class Commands; class Base; class Swapchain;

    struct PhongPushConstants
    {
        // Camera world position
        Vector3 cameraPos;
        float padding;
    };

    class PhongAndUiTechnique
    {
        public:
            PhongAndUiTechnique(nc::graphics::Graphics2* graphics, vk::RenderPass* renderPass);
            ~PhongAndUiTechnique();
            
            void Bind(vk::CommandBuffer* cmd);
            void Record(vk::CommandBuffer* cmd, std::span<nc::MeshRenderer> renderers);
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