#pragma once

#include "graphics\Initializers.h"
#include "graphics\TechniqueType.h"
#include "graphics\techniques\PhongAndUiTechnique.h"
#include "graphics\techniques\WireframeTechnique.h"
#include "graphics\techniques\ShadowMappingTechnique.h"
#include "graphics\Resources\DepthStencil.h"

#include <functional>
#include <unordered_map>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc
{
    class MeshRenderer;
    struct AssetServices;
    namespace particle { class ParticleRenderer; }
}

namespace nc::graphics
{
    class Commands;
    class ShaderResourceServices;
    struct PerFrameRenderState;

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
            Renderer(graphics::Graphics* graphics,
                     AssetServices* assets, 
                     ShaderResourceServices* shaderResources,
                     Vector2 dimensions);

            ~Renderer() noexcept;
            
            void Record(Commands* commands, registry_type* registry);
            void BeginRenderPass(vk::CommandBuffer* cmd, vk::RenderPass* renderPass, vk::Framebuffer& framebuffer, const vk::Extent2D& extent, ClearValue clearValue);
            void Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex);
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
            void InitializeTechniques();
            void RecordUi(vk::CommandBuffer* cmd);

            graphics::Graphics* m_graphics;
            AssetServices* m_assets;
            ShaderResourceServices* m_shaderResources;
            Vector2 m_dimensions;
            
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