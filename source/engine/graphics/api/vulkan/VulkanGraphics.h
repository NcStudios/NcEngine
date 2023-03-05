#pragma once

#include "graphics/IGraphics.h"

#include "ncmath/Vector.h"

#include <memory>
#include <mutex>
#include <vulkan/vulkan.hpp>

namespace nc
{
struct GpuAccessorSignals;
class Registry;

namespace graphics
{
class Device;
class FrameManager;
class GpuAllocator;
struct GpuAssetsStorage;
class Imgui;
class Instance;
class Lighting;
struct PerFrameRenderState;
class PointLight;
class Swapchain;
class RenderGraph;
class ShaderDescriptorSets;
struct ShaderResources;

namespace vulkan
{
class VulkanGraphics : public IGraphics
{
    public:
        VulkanGraphics(const config::ProjectSettings& projectSettings,
                       const config::GraphicsSettings& graphicsSettings,
                       const GpuAccessorSignals& gpuAccessorSignals,
                       uint32_t apiVersion, Registry* registry, HWND hwnd,
                       HINSTANCE hinstance, Vector2 dimensions);

        ~VulkanGraphics() noexcept;

        void InitializeUI() override;
        auto FrameBegin() -> bool override;
        void Draw(const PerFrameRenderState& state) override;
        void FrameEnd() override;
        void OnResize(float width, float height, const WPARAM windowArg) override;
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
        std::unique_ptr<Imgui> m_imgui;
        std::unique_ptr<FrameManager> m_frameManager;
        std::unique_ptr<Lighting> m_lighting;

        std::mutex m_resizingMutex;
        uint32_t m_imageIndex;
        Vector2 m_dimensions;
        bool m_isMinimized;
};
} // namespace vulkan
} // namespace nc::graphics
} // namespace nc

