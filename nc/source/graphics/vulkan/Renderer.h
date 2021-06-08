#pragma once

#include "graphics\vulkan\TechniqueType.h"
#include "graphics\vulkan\MeshManager.h"
#include "graphics\vulkan\TextureManager.h"
#include "graphics\vulkan\techniques\PhongAndUiTechnique.h"
#include "graphics\vulkan\techniques\WireframeTechnique.h"

#include <unordered_map>
#include "vulkan/vulkan.hpp"

namespace nc::vulkan { class MeshRenderer; }

namespace nc::graphics::vulkan
{
    class Commands;
    
    class Renderer
    {
        public:
            Renderer(graphics::Graphics2* graphics);
            ~Renderer();
            
            void Record(Commands* commands);
            void RegisterMeshRenderer(TechniqueType technique, nc::vulkan::MeshRenderer* renderer);
            void BeginRenderPass(vk::CommandBuffer* cmd, vulkan::Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index);

        private:
            void RecordMeshRenderers(vk::CommandBuffer* cmd, vk::PipelineLayout* pipeline, std::unordered_map<std::string, std::vector<nc::vulkan::MeshRenderer*>>* meshRenderers);
            void RecordUi(vk::CommandBuffer* cmd);

            graphics::Graphics2* m_graphics;
            TextureManager m_textureManager;
            MeshManager m_meshManager;
            vk::RenderPass m_mainRenderPass;

            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
    };
}