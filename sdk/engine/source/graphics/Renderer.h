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
    class AssetsSink;
    class Commands;
    class Graphics;
    struct PerFrameRenderState;
    class RenderPassManager;
    class RenderTarget;
    class ShaderResourceServices;

    class Renderer
    {
        public:
            Renderer(graphics::Graphics* graphics,
                     ShaderResourceServices* shaderResources,
                     Vector2 dimensions);

            ~Renderer() noexcept;
            
            void Record(Commands* commands, const PerFrameRenderState& state, AssetServices* assetServices, AssetsSink* assetsSink, uint32_t currentSwapChainImageIndex);
            void Clear() noexcept;
            void InitializeImgui();

        private:
            void RegisterTechniques();
            void RegisterRenderPasses();
            vk::CommandBuffer* BeginFrame(Commands* commands, AssetServices* assetServices, AssetsSink* assetsSink, uint32_t currentSwapChainImageIndex);

            graphics::Graphics* m_graphics;
            ShaderResourceServices* m_shaderResources;
            std::unique_ptr<RenderPassManager> m_renderPasses;
            Vector2 m_dimensions;
            std::unique_ptr<RenderTarget> m_depthStencil;
            std::unique_ptr<RenderTarget> m_colorBuffer;
    };
}