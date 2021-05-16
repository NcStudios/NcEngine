#pragma once

#include "graphics\vulkan\TechniqueType.h"
#include "graphics\vulkan\MeshManager.h"
#include "graphics\vulkan\TextureManager.h"
#include "graphics\vulkan\techniques\PhongAndUiTechnique.h"

#include <unordered_map>
#include "vulkan/vulkan.hpp"

namespace nc::graphics { class Graphics2; }
namespace nc::vulkan { class MeshRenderer; }

namespace nc::graphics::vulkan
{
    class Commands;
    
    class Renderer
    {
        public:
            Renderer(graphics::Graphics2* graphics);
            void Record(Commands* commands);
            void RegisterMeshRenderer(TechniqueType technique, nc::vulkan::MeshRenderer* renderer);

        private:
            void RecordMeshRenderers(vk::CommandBuffer* cmd, vk::PipelineLayout* pipeline, std::unordered_map<std::string, std::vector<nc::vulkan::MeshRenderer*>>* meshRenderers);
            void RecordUi(vk::CommandBuffer* cmd);

            graphics::Graphics2* m_graphics;
            TextureManager m_textureManager;
            MeshManager m_meshManager;

            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
    };
}