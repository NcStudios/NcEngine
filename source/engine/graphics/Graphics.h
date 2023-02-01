#pragma once

#include "assets/AssetManagers.h"
#include "camera/MainCamera.h"
#include "ecs/Registry.h"
#include "GpuAllocator.h"
#include "ncmath/Vector.h"
#include "platform/win32/NcWin32.h"
#include "DirectXMath.h"

#include <array>
#include <memory>
#include <mutex>

namespace nc { struct AssetServices; class Registry; }

namespace nc::graphics
{
    class GpuOptions;
    class Commands;
    struct Core;
    class FrameManager;
    struct GpuAssetsStorage;
    class Lighting;
    struct PerFrameRenderState;
    class PointLight;
    class Swapchain;
    class Imgui;
    class RenderGraph;
    class ShaderDescriptorSets;
    struct ShaderResources;

    /** @todo: Plan is to have Graphics header be general enough to be implemented by Vulkan or DX, and eventually have a Vulkan (and potentially DX) version of Renderer, FrameManager, etc. Slowly working towards that. */
    class Graphics
    {
        public:
            Graphics(camera::MainCamera* mainCamera, Registry* registry, const nc::GpuAccessorSignals& gpuAccessorSignals, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);
            ~Graphics() noexcept;

            void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);
            void Clear();
            void InitializeUI();

            bool FrameBegin();
            void Draw(const PerFrameRenderState& state);
            void FrameEnd();

        private:
            void Resize();

            camera::MainCamera* m_mainCamera;
            Registry* m_registry;
            std::unique_ptr<Core> m_core;
            std::unique_ptr<GpuOptions> m_gpuOptions;
            std::unique_ptr<Swapchain> m_swapchain;
            std::unique_ptr<Commands> m_commands;
            std::unique_ptr<GpuAllocator> m_allocator;
            std::unique_ptr<ShaderDescriptorSets> m_shaderDescriptorSets;
            std::unique_ptr<ShaderResources> m_shaderResources;
            std::unique_ptr<AssetServices> m_assetServices;
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