#pragma once

#include "MainCamera.h"
#include "math/Vector.h"
#include "platform/win32/NcWin32.h"
#include "directx/math/DirectXMath.h"

#include <memory>
#include <mutex>

namespace nc { struct AssetServices; }

namespace nc::graphics
{
    class Base;
    class Commands;
    class Swapchain;
    class DepthStencil;
    class Renderer;
    class RenderPassManager;
    struct ShaderResourceServices;

    class Graphics
    {
        public:
            Graphics(MainCamera* mainCamera, HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);
            ~Graphics() noexcept;
            Graphics(const Graphics&) = delete;
            Graphics(Graphics&&) = delete;
            Graphics& operator=(const Graphics&) = delete;
            Graphics& operator=(Graphics&&) = delete;

            void ResizeTarget(float width, float height);
            void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);
            void ToggleFullscreen();
            void SetClearColor(std::array<float, 4> color);

            Base* GetBasePtr() const noexcept;
            Swapchain* GetSwapchainPtr() const noexcept;
            Commands* GetCommandsPtr() const noexcept;
            Renderer* GetRendererPtr() const noexcept;
            const Vector2 GetDimensions() const noexcept;
            const DepthStencil& GetDepthStencil() const noexcept;
            const std::array<float, 4>& GetClearColor() const noexcept;

            // Blocks the current thread until all operations in the command queues on the device are completed. 
            void WaitIdle();
            void Clear();
            void InitializeUI();

            uint32_t FrameBegin();
            void Draw();
            void FrameEnd();

    #ifdef NC_EDITOR_ENABLED
            uint32_t GetDrawCallCount() const;
            void IncrementDrawCallCount();
    #endif

        private:
            void RecreateSwapchain(Vector2 dimensions);
            bool GetNextImageIndex(uint32_t* imageIndex);
            void RenderToImage(uint32_t imageIndex);
            bool PresentImage(uint32_t imageIndex);

            MainCamera* m_mainCamera;
            std::unique_ptr<Base> m_base;
            std::unique_ptr<DepthStencil> m_depthStencil;
            std::unique_ptr<Swapchain> m_swapchain;
            std::unique_ptr<Commands> m_commands;
            std::unique_ptr<ShaderResourceServices> m_shaderResources;
            std::unique_ptr<Renderer> m_renderer;

            std::mutex m_resizingMutex;
            uint32_t m_imageIndex;
            Vector2 m_dimensions;
            bool m_isMinimized;
            std::array<float, 4> m_clearColor;
            uint32_t m_drawCallCount = 0;
    };
}