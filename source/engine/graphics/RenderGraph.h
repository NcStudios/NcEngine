#pragma once

#include "graphics/GpuAssetsStorage.h"
#include "graphics/renderpasses/RenderPass.h"
#include "graphics/techniques/ShadowMappingTechnique.h"

#include <concepts>
#include <string>
#include <typeinfo>

namespace nc::graphics
{
class GpuAllocator;
class GpuOptions;
class PerFrameGpuContext;
class ShaderDescriptorSets;
class Swapchain;

class RenderGraph
{
    public:
        RenderGraph(vk::Device device, Swapchain* swapchain, GpuOptions* gpuOptions, GpuAllocator* gpuAllocator, ShaderDescriptorSets* descriptorSets, Vector2 dimensions);
        ~RenderGraph() noexcept;

        RenderPass* Acquire(const std::string& uid);
        void Add(std::unique_ptr<RenderPass> renderPass);
        void Execute(PerFrameGpuContext* currentFrame, const PerFrameRenderState& frameData, const MeshStorage& meshStorage, uint32_t frameBufferIndex, Vector2 dimensions);
        void Remove(const std::string& uid);
        void Resize(Swapchain* swapchain, const Vector2& dimensions);

        template <std::derived_from<ITechnique> T>
        void RegisterTechnique(const std::string& uid);
        void RegisterShadowMappingTechnique(const std::string& uid, uint32_t shadowCasterIndex);
        
        template <std::derived_from<ITechnique> T>
        void UnregisterTechnique(const std::string& uid);


    private:
        vk::Device m_device;
        Swapchain* m_swapchain;
        GpuOptions* m_gpuOptions;
        GpuAllocator* m_gpuAllocator;
        ShaderDescriptorSets* m_descriptorSets;
        std::vector<std::unique_ptr<RenderPass>> m_renderPasses;
        Vector2& m_dimensions;
};

template <std::derived_from<ITechnique> T>
void RenderGraph::RegisterTechnique(const std::string& uid)
{
    UnregisterTechnique<T>(uid);
    auto* renderPass = Acquire(uid);
    renderPass->techniques.push_back(std::make_unique<T>(m_device, m_gpuOptions, m_descriptorSets, &(renderPass->renderPass.get())));
}

template <std::derived_from<ITechnique> T>
void RenderGraph::UnregisterTechnique(const std::string& uid)
{
    const auto& techniqueType = typeid(T);
    auto* renderPass = Acquire(uid);
    auto techniquePos = std::ranges::find_if(renderPass->techniques, [&techniqueType](const auto& foundTechnique)
    {
        return (typeid(foundTechnique) == techniqueType);
    });

    if (techniquePos != renderPass->techniques.end())
    {
        *techniquePos = std::move(renderPass->techniques.back());
        renderPass->techniques.pop_back();
    }
}
} // namespace nc