#pragma once

#include "component/Component.h"
#include "vulkan/vk_mem_alloc.hpp"
#include "directx/math/DirectXMath.h"

#include <vector>
#include <span>

namespace nc { struct MeshView; }

namespace nc::graphics
{
    class Graphics; class Commands; class Base; class Swapchain;

    struct PhongPushConstants
    {
        // Camera world position
        Vector3 cameraPos;
        float padding;
    };

    class PhongAndUiTechnique
    {
        public:
            PhongAndUiTechnique(nc::graphics::Graphics* graphics, vk::RenderPass* renderPass);
            ~PhongAndUiTechnique() noexcept;
            
            void Bind(vk::CommandBuffer* cmd);
            void Record(vk::CommandBuffer* cmd, const Vector3& cameraPosition, std::span<const MeshView> meshes);
            void Clear() noexcept;

        private:
            void CreatePipeline(vk::RenderPass* renderPass);

            nc::graphics::Graphics* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            vk::UniquePipeline m_pipeline;
            vk::UniquePipelineLayout m_pipelineLayout;
    };
}