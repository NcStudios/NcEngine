#pragma once

#include "graphics/GpuAssetsStorage.h"
#include "graphics/vk/Initializers.h"

#include <functional>
#include <unordered_map>
#include "vulkan/vk_mem_alloc.hpp"

namespace nc
{
class MeshRenderer;
struct AssetServices;

namespace particle
{
class ParticleRenderer;
}

namespace graphics
{
class Commands;
class Graphics;
class PerFrameGpuContext;
class RenderTarget;
class ShaderResourceServices;
struct PerFrameRenderState;
class RenderPassManager;

class Renderer
{
    public:
        Renderer(graphics::Graphics* graphics,
                 vk::Device device, 
                 ShaderResourceServices* shaderResources,
                 Vector2 dimensions);
        ~Renderer() noexcept;

        void Record(PerFrameGpuContext* currentFrame, const PerFrameRenderState& state, const MeshStorage& meshStorage, uint32_t currentSwapChainImageIndex);
        void Clear() noexcept;
        void InitializeImgui(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, Commands* commands, uint32_t maxSamplesCount); /** @todo: Where should Imgui stuff live? */

    private:
        void BindMeshBuffers(vk::CommandBuffer* cmd, const VertexBuffer& vertexData, const IndexBuffer& indexData);
        void RegisterTechniques(vk::Device device);
        void RegisterRenderPasses();

        graphics::Graphics* m_graphics;
        ShaderResourceServices* m_shaderResources;
        std::unique_ptr<RenderPassManager> m_renderPasses;
        Vector2 m_dimensions;
        std::unique_ptr<RenderTarget> m_depthStencil;
        std::unique_ptr<RenderTarget> m_colorBuffer;
        vk::UniqueDescriptorPool m_imguiDescriptorPool;
};
} // namespace nc
} // namespace graphics