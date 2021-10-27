#pragma once

#include "graphics\Initializers.h"

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
            
            void Record(Commands* commands, const PerFrameRenderState& state, uint32_t currentSwapChainImageIndex);

            void Clear() noexcept;
            void InitializeImgui();

        private:
            void RegisterTechniques();
            void RegisterRenderPasses();
            vk::CommandBuffer* BeginFrame(Commands* commands, uint32_t currentSwapChainImageIndex);

            graphics::Graphics* m_graphics;
            AssetServices* m_assets;
            ShaderResourceServices* m_shaderResources;
            std::unique_ptr<RenderPassManager> m_renderPasses;
            Vector2 m_dimensions;
            
    };
}