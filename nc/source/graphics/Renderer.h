#pragma once

#include "graphics\TechniqueType.h"
#include "graphics\TextureManager.h"
#include "graphics\techniques\PhongAndUiTechnique.h"
#include "graphics\techniques\WireframeTechnique.h"
// #include "graphics\techniques\ParticleTechnique.h"

#include <unordered_map>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc { class MeshRenderer; }
namespace nc::particle { class ParticleRenderer; }

namespace nc::graphics
{
    class Commands;
    struct PerFrameRenderState;
    
    class Renderer
    {
        public:
            Renderer(graphics::Graphics* graphics);
            ~Renderer() noexcept;
            
            void BeginRenderPass(vk::CommandBuffer* cmd, Swapchain* swapchain, vk::RenderPass* renderPass, uint32_t index);
            void Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex);
            void BindSharedData(vk::CommandBuffer* cmd);
            void Clear() noexcept;

            // void RegisterParticleEmitter(std::vector<particle::EmitterState>* m_emitterStates);
            void ClearParticleEmitters();

            #ifdef NC_EDITOR_ENABLED
            void RegisterDebugWidget(nc::DebugWidget widget);
            void ClearDebugWidget();
            #endif

        private:
            void RecordUi(vk::CommandBuffer* cmd);

            graphics::Graphics* m_graphics;
            TextureManager m_textureManager;
            vk::RenderPass m_mainRenderPass;

            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
            #endif
            // std::unique_ptr<ParticleTechnique> m_particleTechnique;
    };
}