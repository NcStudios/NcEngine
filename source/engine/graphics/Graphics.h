#pragma once

#include "assets/AssetManagers.h"
#include "camera/MainCamera.h"
#include "GpuAllocator.h"
#include "math/Vector.h"
#include "platform/win32/NcWin32.h"
#include "DirectXMath.h"
#ifdef NC_DEBUG_RENDERING_ENABLED
#include "DebugRenderer.h"
#endif

#include <array>
#include <memory>
#include <mutex>

namespace nc { struct AssetServices; }

namespace nc::graphics
{
    class GpuOptions;
    class Commands;
    struct Core;
    class FrameManager;
    struct GpuAssetsStorage;
    struct PerFrameRenderState;
    class Swapchain;
    class Renderer;
    class RenderPassManager;
    class ShaderResourceServices;

    /** @todo: Plan is to have Graphics header be general enough to be implemented by Vulkan or DX, and eventually have a Vulkan (and potentially DX) version of Renderer, FrameManager, etc. Slowly working towards that. */
    class Graphics
    {
        public:
            Graphics(camera::MainCamera* mainCamera, const nc::GpuAccessorSignals& gpuAccessorSignals, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);
            ~Graphics() noexcept;
            Graphics(const Graphics&) = delete;
            Graphics(Graphics&&) = delete;
            Graphics& operator=(const Graphics&) = delete;
            Graphics& operator=(Graphics&&) = delete;

            void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);
            void SetClearColor(std::array<float, 4> color);
            void Clear();
            void InitializeUI();

            GpuOptions* GetBasePtr() const noexcept;
            GpuAllocator* GetAllocatorPtr() const noexcept;
            Swapchain* GetSwapchainPtr() const noexcept;
            ShaderResourceServices* GetShaderResources() const noexcept;
            const Vector2 GetDimensions() const noexcept;
            const std::array<float, 4>& GetClearColor() const noexcept;

            #ifdef NC_DEBUG_RENDERING_ENABLED
            graphics::DebugData* GetDebugData();
            #endif

            bool FrameBegin();
            void Draw(const PerFrameRenderState& state);
            void FrameEnd();

        private:
            void RecreateSwapchain(Vector2 dimensions);

            camera::MainCamera* m_mainCamera;
            std::unique_ptr<Core> m_engine;
            std::unique_ptr<GpuOptions> m_base;
            std::unique_ptr<Swapchain> m_swapchain;
            std::unique_ptr<Commands> m_commands;
            std::unique_ptr<GpuAllocator> m_allocator;
            std::unique_ptr<ShaderResourceServices> m_shaderResources;
            std::unique_ptr<AssetServices> m_assetServices;
            std::unique_ptr<GpuAssetsStorage> m_gpuAssetsStorage;
            #ifdef NC_DEBUG_RENDERING_ENABLED
            graphics::DebugRenderer m_debugRenderer;
            #endif
            std::unique_ptr<Renderer> m_renderer;
            std::unique_ptr<FrameManager> m_frameManager;

            std::mutex m_resizingMutex;
            uint32_t m_imageIndex;
            Vector2 m_dimensions;
            bool m_isMinimized;
            std::array<float, 4> m_clearColor;
    };
} // namespace nc::graphics