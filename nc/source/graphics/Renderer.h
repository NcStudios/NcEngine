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
    class RenderPassManager;

    class Renderer
    {
        public:
            Renderer(graphics::Graphics* graphics,
                     AssetServices* assets, 
                     ShaderResourceServices* shaderResources,
                     Vector2 dimensions);

            ~Renderer() noexcept;
            
            void Resize(Vector2 dimensions, vk::Extent2D extent);
            void RegisterTechniques();
            void RegisterRenderPasses();
            void Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex);
            void Clear() noexcept;
            void InitializeImgui();

            #ifdef NC_EDITOR_ENABLED
            void RegisterDebugWidget(nc::DebugWidget widget);
            void ClearDebugWidget();
            #endif

        private:
            void BindSharedData(vk::CommandBuffer* cmd);
            void RecordUi(vk::CommandBuffer* cmd);
            graphics::Graphics* m_graphics;
            AssetServices* m_assets;
            ShaderResourceServices* m_shaderResources;
            std::unique_ptr<RenderPassManager> m_renderPasses;
            Vector2 m_dimensions;
            
            std::unique_ptr<PhongAndUiTechnique> m_phongAndUiTechnique;
            std::unique_ptr<ShadowMappingTechnique> m_shadowMappingTechnique;
            #ifdef NC_EDITOR_ENABLED
            std::unique_ptr<WireframeTechnique> m_wireframeTechnique;
            #endif
    };
}