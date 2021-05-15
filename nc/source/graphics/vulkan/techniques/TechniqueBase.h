#pragma once

#include "graphics/vulkan/TechniqueType.h"
#include "graphics/vulkan/Mesh.h"
#include "graphics/vulkan/Texture.h"

#include "vulkan/vulkan.hpp"
#include <vector>

namespace nc 
{ 
    class Transform; 
    namespace vulkan
    { 
        class MeshRenderer; 
    }
}

namespace nc::graphics
{
    class Graphics2;
    namespace vulkan { class Base; class Swapchain; class Commands; struct GlobalData; }
}

namespace nc::graphics::vulkan
{
    using namespace nc::vulkan;

    class TechniqueBase
    {
        public:
            TechniqueBase(TechniqueType techniqueType, nc::graphics::Graphics2* graphics);
            vk::ShaderModule CreateShaderModule(const std::vector<uint32_t>& code, vulkan::Base* base);
            static std::vector<uint32_t> ReadShader(const std::string& filename);
            TechniqueType GetType() const noexcept;
            void RegisterMeshRenderer(MeshRenderer* renderer);
            virtual ~TechniqueBase() noexcept;
            virtual void Record(Commands* commands) = 0;

        protected:
            nc::graphics::Graphics2* m_graphics;
            Base* m_base;
            Swapchain* m_swapchain;
            std::unordered_map<std::string, std::vector<MeshRenderer*>> m_meshRenderers;
            vk::Pipeline m_pipeline;
            vk::PipelineLayout m_pipelineLayout;
            std::vector<vk::RenderPass> m_renderPasses;
            vk::DescriptorSetLayout m_descriptorSetLayout;
            TechniqueType m_type;
    };
}