#pragma once

#include "ecs/Registry.h"
#include "renderpasses/RenderPass.h"

#include "vulkan/vk_mem_alloc.hpp"

namespace nc::graphics
{
class GpuAllocator;
class PointLight;
class RenderGraph;
class ShaderDescriptorSets;
struct ShaderResources;
class Swapchain;

class Lighting
{
    public:
        Lighting(Registry* registry, 
                 vk::Device device, 
                 GpuAllocator* allocator, 
                 GpuOptions* gpuOptions,
                 Swapchain* swapchain,
                 RenderGraph* renderGraph,
                 ShaderDescriptorSets* shaderDescriptorSets,
                 ShaderResources* shaderResources,
                 Vector2 dimensions);
        void Resize(const Vector2& dimensions);
        void Clear();

    private:
        void OnAddPointLightConnection();
        void OnRemovePointLightConnection();
        auto CreateShadowMappingPass(nc::Vector2 dimensions, uint32_t index) -> nc::graphics::RenderPass;

        Registry* m_registry;
        vk::Device m_device;
        GpuAllocator* m_allocator;
        GpuOptions* m_gpuOptions;
        Swapchain* m_swapchain;
        RenderGraph* m_renderGraph;
        ShaderDescriptorSets* m_shaderDescriptorSets;
        ShaderResources* m_shaderResources;
        Vector2 m_dimensions;
        Connection<PointLight&> m_onAddPointLightConnection;
        Connection<Entity> m_onRemovePointLightConnection;
        uint32_t m_numShadowCasters;
        std::vector<std::string> m_ids;
};
}