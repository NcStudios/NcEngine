#pragma once

#include "assets/AssetManagers.h"
#include "GpuAllocator.h"
#include "ncmath/Vector.h"
#include "platform/win32/NcWin32.h"

#include <memory>
#include <mutex>

namespace nc {class Registry;}
namespace nc::graphics
{
    class Device;
    class GpuOptions;
    class Instance;
    class FrameManager;
    class GpuAllocator;
    struct GpuAssetsStorage;
    class Lighting;
    struct PerFrameRenderState;
    class PointLight;
    class Swapchain;
    class Imgui;
    class RenderGraph;
    class ShaderDescriptorSets;
    struct ShaderResources;

    class Graphics
    {
        public:
            Graphics(Registry* registry, const nc::GpuAccessorSignals& gpuAccessorSignals,
                     const std::string& appName, uint32_t appVersion, uint32_t apiVersion,
                     bool useValidationLayers, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);

            ~Graphics() noexcept;

            void OnResize(float width, float height, const WPARAM windowArg);
            void Clear();
            void InitializeUI() const;

            auto FrameBegin() -> bool;
            void Draw(const PerFrameRenderState& state);
            void FrameEnd();

        private:
            void Resize(const Vector2& dimensions);

            std::unique_ptr<Instance> m_instance;
            vk::UniqueSurfaceKHR m_surface;
            std::unique_ptr<Device> m_device;
            std::unique_ptr<GpuOptions> m_gpuOptions;
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
} // namespace nc::graphics