#pragma once

#include "graphics/vulkan/TechniqueType.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"

#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc { class Transform; }
namespace nc::graphics
{
    class Graphics2;
    namespace vulkan { class Base; class Swapchain; class Commands; struct GlobalData; }
}

namespace nc::graphics::vulkan
{
    class TechniqueBase
    {
        public:
            TechniqueBase(TechniqueType techniqueType, nc::graphics::Graphics2* graphics);
            TechniqueBase(TechniqueType techniqueType, GlobalData* globalData, nc::graphics::Graphics2* graphics);
            vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, vulkan::Base* base);
            static std::vector<uint32_t> ReadShader(const std::string& filename);
            TechniqueType GetType() const noexcept;
            void RegisterMeshRenderer(Mesh mesh, Texture texture, Transform* transform);
            virtual ~TechniqueBase() noexcept;
            virtual void Record(Commands* commands) = 0;

        protected:
            nc::graphics::Graphics2* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            GlobalData* m_globalData;
            std::vector<Mesh> m_meshes;
            std::vector<Texture> m_textures;
            std::unordered_map<std::string, std::vector<Transform*>> m_objects;
            vk::Pipeline m_pipeline;
            vk::PipelineLayout m_pipelineLayout;
            std::vector<vk::RenderPass> m_renderPasses;
            vk::DescriptorSetLayout m_descriptorSetLayout;
            TechniqueType m_type;
    };
}