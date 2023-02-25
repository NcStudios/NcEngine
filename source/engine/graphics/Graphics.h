#pragma once

#include "assets/AssetManagers.h"
#include "GpuAllocator.h"
#include "ncmath/Vector.h"
#include "platform/win32/NcWin32.h"
#include "DirectXMath.h"

#include <array>
#include <memory>
#include <mutex>

namespace nc::graphics
{
    class Device;
    class GpuOptions;
    class Instance;
    class FrameManager;
    struct GpuAssetsStorage;
    struct PerFrameRenderState;
    class Swapchain;
    class Renderer;
    class RenderPassManager;
    class ShaderResources;

    /** @todo: Plan is to have Graphics header be general enough to be implemented by Vulkan or DX, and eventually have a Vulkan (and potentially DX) version of Renderer, FrameManager, etc. Slowly working towards that. */
    class Graphics
    {
        public:
            Graphics(const nc::GpuAccessorSignals& gpuAccessorSignals, const std::string& appName,
                     uint32_t appVersion, uint32_t apiVersion, bool useValidationLayers,
                     HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);

            ~Graphics() noexcept;

            void OnResize(float width, float height, WPARAM windowArg);
            void Clear();
            void InitializeUI();

            bool FrameBegin();
            void Draw(const PerFrameRenderState& state);
            void FrameEnd();

        private:
            void RecreateSwapchain(Vector2 dimensions);

            std::unique_ptr<Instance> m_instance;
            vk::UniqueSurfaceKHR m_surface;
            std::unique_ptr<Device> m_device;
            std::unique_ptr<GpuOptions> m_gpuOptions;
            std::unique_ptr<Swapchain> m_swapchain;
            std::unique_ptr<GpuAllocator> m_allocator;
            std::unique_ptr<ShaderResources> m_shaderResources;
            std::unique_ptr<GpuAssetsStorage> m_gpuAssetsStorage;
            std::unique_ptr<Renderer> m_renderer;
            std::unique_ptr<FrameManager> m_frameManager;

            std::mutex m_resizingMutex;
            uint32_t m_imageIndex;
            Vector2 m_dimensions;
            bool m_isMinimized;
    };
} // namespace nc::graphics