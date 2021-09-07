#pragma once

#include "ECS.h"
#include "graphics\vulkan\TechniqueType.h"
#include "graphics\vulkan\MeshManager.h"
#include "graphics\vulkan\TextureManager.h"
#include "graphics\vulkan\techniques\ParticleTechnique.h"
#include "graphics\vulkan\techniques\PhongAndUiTechnique.h"
#include "graphics\vulkan\techniques\ShadowMappingTechnique.h"
#include "graphics\vulkan\techniques\WireframeTechnique.h"
#include "graphics\vulkan\Resources\DepthStencil.h"

#include <unordered_map>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc::vulkan { class MeshRenderer; }
namespace nc::particle::vulkan { class ParticleRenderer; }

namespace nc::graphics::vulkan
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
            Renderer(graphics::Graphics2* graphics);
            ~Renderer();
            
            void BeginRenderPass(vk::CommandBuffer* cmd, vulkan::Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index);
            void Record(Commands* commands, registry_type* registry);
            void BindSharedData(vk::CommandBuffer* cmd);
            void Clear();

            void RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates);
            void ClearParticleEmitters();

            #ifdef NC_EDITOR_ENABLED
            void RegisterDebugWidget(nc::vulkan::DebugWidget widget);
            void ClearDebugWidget();
            #endif

            void DeregisterRenderable(Entity entity);

        private:
            void RecordUi(vk::CommandBuffer* cmd);
            void InitializeShadowMappingRenderPass(vulkan::Swapchain* swapchain);

            graphics::Graphics2* m_graphics;
            TextureManager m_textureManager;
            MeshManager m_meshManager;
            vk::RenderPass m_mainRenderPass;
            ShadowMappingPass m_shadowMappingPass;

            std::vector<std::pair<Entity, std::vector<Entity>*>> m_storageHandles;
            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
            #endif
            std::unique_ptr<ParticleTechnique> m_particleTechnique;
            std::unique_ptr<ShadowMappingTechnique> m_shadowMappingTechnique;
    };
}