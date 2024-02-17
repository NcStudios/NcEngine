#pragma once

#include "graphics/IGraphics.h"

#include "ncmath/Vector.h"

#include <memory>
#include <mutex>
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace nc
{
class Registry;

namespace asset
{
class NcAsset;
} // namespace asset

namespace graphics
{
class Device;
class FrameManager;
class GpuAllocator;
class Imgui;
class Instance;
class Lighting;
struct PerFrameRenderState;
class PointLight;
class Swapchain;
class RenderGraph;
class ShaderDescriptorSets;
struct ShaderResources;
struct ShaderResourceBus;

namespace vulkan
{
struct GpuAssetsStorage;
struct GpuShaderStorage;

class VulkanGraphics : public IGraphics
{
    public:
        VulkanGraphics(const config::ProjectSettings& projectSettings,
                       const config::GraphicsSettings& graphicsSettings,
                       const config::MemorySettings& memorySettings,
                       asset::NcAsset* assetModule,
                       ShaderResourceBus& shaderResourceBus,
                       uint32_t apiVersion, Registry* registry, 
                       GLFWwindow* window, Vector2 dimensions, Vector2 screenExtent);

        ~VulkanGraphics() noexcept;

        void CommitResourceLayout() override;
        auto FrameBegin() -> bool override;
        auto CurrentFrameIndex() -> uint32_t override;
        void Draw(const PerFrameRenderState& state) override;
        void FrameEnd() override;
        void OnResize(float width, float height, bool isMinimized) override;
        void Clear() noexcept override;

    private:
        void Resize(const Vector2& dimensions);

        std::unique_ptr<Instance> m_instance;
        vk::UniqueSurfaceKHR m_surface;
        std::unique_ptr<Device> m_device;
        std::unique_ptr<Swapchain> m_swapchain;
        std::unique_ptr<GpuAllocator> m_allocator;
        std::unique_ptr<ShaderDescriptorSets> m_shaderDescriptorSets;
        std::unique_ptr<ShaderResources> m_shaderResources;
        std::unique_ptr<GpuAssetsStorage> m_gpuAssetsStorage;
        std::unique_ptr<RenderGraph> m_renderGraph;
        std::unique_ptr<GpuShaderStorage> m_gpuShaderStorage;
        std::unique_ptr<Imgui> m_imgui;
        std::unique_ptr<FrameManager> m_frameManager;
        std::unique_ptr<Lighting> m_lighting;

        std::mutex m_resizingMutex;
        uint32_t m_imageIndex;
        Vector2 m_dimensions;
        Vector2 m_screenExtent;
        bool m_isMinimized;
};
} // namespace vulkan
} // namespace graphics
} // namespace nc

