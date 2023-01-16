#pragma once

#include "graphics/GpuAssetsStorage.h"
#include "graphics/Initializers.h"
#include "ecs/Registry.h"

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
class Swapchain;
class GpuOptions;
struct PerFrameRenderState;
class PerFrameGpuContext;
class PointLight;
class RenderPassManager;
class RenderTarget;
class ShaderResources;

class Renderer
{
    public:
        Renderer(vk::Device device, Registry* registry, Swapchain* swapchain, GpuOptions* gpuOptions, GpuAllocator* gpuAllocator, ShaderResources* shaderResources, Vector2 dimensions);
        ~Renderer() noexcept;

        void Record(PerFrameGpuContext* currentFrame, const PerFrameRenderState& state, const MeshStorage& meshStorage, uint32_t currentSwapChainImageIndex);
        void Clear() noexcept;
        void InitializeImgui(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, Commands* commands, uint32_t maxSamplesCount); /** @todo: Where should Imgui stuff live? */

    private:
        void BindMeshBuffers(vk::CommandBuffer* cmd, const VertexBuffer& vertexData, const IndexBuffer& indexData);
        void AddShadowMappingPass();
        void RemoveShadowMappingPass();

        vk::Device m_device;
        Swapchain* m_swapchain;
        GpuOptions* m_gpuOptions;
        ShaderResources* m_shaderResources;
        std::unique_ptr<RenderPassManager> m_renderPasses;
        Vector2 m_dimensions;
        std::unique_ptr<RenderTarget> m_depthStencil;
        std::unique_ptr<RenderTarget> m_colorBuffer;
        vk::UniqueDescriptorPool m_imguiDescriptorPool;
        Connection<PointLight&> m_onAddPointLightConnection;
        Connection<Entity> m_onRemovePointLightConnection;
        uint32_t m_numShadowCasters;
};
} // namespace nc
} // namespace graphics