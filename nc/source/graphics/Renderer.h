#pragma once

#include "graphics\TechniqueType.h"
#include "graphics\MeshManager.h"
#include "graphics\TextureManager.h"
#include "graphics\techniques\PhongAndUiTechnique.h"
#include "graphics\techniques\WireframeTechnique.h"
#include "graphics\techniques\ShadowMappingTechnique.h"
#include "graphics\Resources\DepthStencil.h"
// #include "graphics\techniques\ParticleTechnique.h"

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
        : renderPass{nullptr},
          frameBuffer{nullptr}
        {
        }

        vk::UniqueRenderPass renderPass;
        vk::UniqueFramebuffer frameBuffer;
    };

    class Renderer
    {
        public:
            Renderer(graphics::Graphics* graphics);
            ~Renderer() noexcept;
            
            void Record(Commands* commands, registry_type* registry);
            void BeginRenderPass(vk::CommandBuffer* cmd, Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index);
            void BindSharedData(vk::CommandBuffer* cmd);
            void Clear() noexcept;

            // void RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates);
            void ClearParticleEmitters();

            #ifdef NC_EDITOR_ENABLED
            void RegisterDebugWidget(nc::DebugWidget widget);
            void ClearDebugWidget();
            #endif
            void InitializeShadowMappingRenderPass();

        private:
            void RecordUi(vk::CommandBuffer* cmd);

            graphics::Graphics* m_graphics;
            TextureManager m_textureManager;
            MeshManager m_meshManager;
            vk::RenderPass m_mainRenderPass;        
            ShadowMappingPass m_shadowMappingPass;

            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
            #endif
            std::unique_ptr<ShadowMappingTechnique> m_shadowMappingTechnique;
            // std::unique_ptr<ParticleTechnique> m_particleTechnique;
    };
}