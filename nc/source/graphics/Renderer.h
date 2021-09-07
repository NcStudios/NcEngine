#pragma once

<<<<<<< HEAD:nc/source/graphics/vulkan/Renderer.h
#include "ECS.h"
#include "graphics\vulkan\TechniqueType.h"
#include "graphics\vulkan\MeshManager.h"
#include "graphics\vulkan\TextureManager.h"
#include "graphics\vulkan\techniques\ParticleTechnique.h"
#include "graphics\vulkan\techniques\PhongAndUiTechnique.h"
#include "graphics\vulkan\techniques\ShadowMappingTechnique.h"
#include "graphics\vulkan\techniques\WireframeTechnique.h"
#include "graphics\vulkan\Resources\DepthStencil.h"
=======
#include "graphics\TechniqueType.h"
#include "graphics\MeshManager.h"
#include "graphics\TextureManager.h"
#include "graphics\techniques\PhongAndUiTechnique.h"
#include "graphics\techniques\WireframeTechnique.h"
// #include "graphics\techniques\ParticleTechnique.h"
>>>>>>> 85b63c05f6ab69ea59f72df903238237ed757169:nc/source/graphics/Renderer.h

#include <unordered_map>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc { class MeshRenderer; }
namespace nc::particle { class ParticleRenderer; }

namespace nc::graphics
{
    class Commands;
    
    struct ShadowMappingPass
    {
        ShadowMappingPass()
        : renderPass{ nullptr},
          frameBuffer{ nullptr},
          sampler{ nullptr},
          depthStencil{ nullptr}
        {
        }

        vk::UniqueRenderPass renderPass;
        vk::UniqueFramebuffer frameBuffer;
        vk::UniqueSampler sampler;
        std::unique_ptr<DepthStencil> depthStencil;
    };

    class Renderer
    {
        public:
            Renderer(graphics::Graphics* graphics);
            ~Renderer();
            
<<<<<<< HEAD:nc/source/graphics/vulkan/Renderer.h
            void BeginRenderPass(vk::CommandBuffer* cmd, vulkan::Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index);
            void Record(Commands* commands, registry_type* registry);
=======
            void BeginRenderPass(vk::CommandBuffer* cmd, Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index);
            void Record(Commands* commands);
>>>>>>> 85b63c05f6ab69ea59f72df903238237ed757169:nc/source/graphics/Renderer.h
            void BindSharedData(vk::CommandBuffer* cmd);
            void Clear();

            // void RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates);
            void ClearParticleEmitters();

            #ifdef NC_EDITOR_ENABLED
            void RegisterDebugWidget(nc::DebugWidget widget);
            void ClearDebugWidget();
            #endif

            void DeregisterRenderable(Entity entity);

        private:
            void RecordUi(vk::CommandBuffer* cmd);
            void InitializeShadowMappingRenderPass(vulkan::Swapchain* swapchain);

            graphics::Graphics* m_graphics;
            TextureManager m_textureManager;
            MeshManager m_meshManager;
            vk::RenderPass m_mainRenderPass;
            ShadowMappingPass m_shadowMappingPass;

            std::vector<std::pair<Entity, std::vector<Entity>*>> m_storageHandles;
            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
            #endif
<<<<<<< HEAD:nc/source/graphics/vulkan/Renderer.h
            std::unique_ptr<ParticleTechnique> m_particleTechnique;
            std::unique_ptr<ShadowMappingTechnique> m_shadowMappingTechnique;
=======
            // std::unique_ptr<ParticleTechnique> m_particleTechnique;
>>>>>>> 85b63c05f6ab69ea59f72df903238237ed757169:nc/source/graphics/Renderer.h
    };
}