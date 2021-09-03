#pragma once

#include "math/Vector.h"
#include "platform/win32/NcWin32.h"
#include "directx/math/DirectXMath.h"
#include <memory>

namespace nc::graphics
{
    namespace vulkan { class Base; class Commands; class Swapchain; class DepthStencil; class Renderer; class ResourceManager; }

    class Graphics2
    {
        public:
            Graphics2(HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);
            ~Graphics2() noexcept;
            Graphics2(const Graphics2&) = delete;
            Graphics2(Graphics2&&) = delete;
            Graphics2& operator=(const Graphics2&) = delete;
            Graphics2& operator=(Graphics2&&) = delete;

            const Vector3 GetCameraPosition() const noexcept;
            DirectX::FXMMATRIX GetViewMatrix() const noexcept;
            DirectX::FXMMATRIX GetProjectionMatrix() const noexcept;

            void SetViewMatrix(DirectX::FXMMATRIX cam) noexcept;
            void SetProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept;
            void SetCameraPosition(Vector3 cameraPosition);

            void ResizeTarget(float width, float height);
            void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);
            void ToggleFullscreen();
            void SetClearColor(std::array<float, 4> color);
            void SetRenderer(vulkan::Renderer* renderer);

            const vulkan::Base& GetBase() const noexcept;
            vulkan::Base* GetBasePtr() const noexcept;
            vulkan::Swapchain* GetSwapchainPtr() const noexcept;
            vulkan::Commands* GetCommandsPtr() const noexcept;
            vulkan::Renderer* GetRendererPtr() const noexcept;
            const Vector2 GetDimensions() const noexcept;
            const std::array<float, 4>& GetClearColor() const noexcept;

            // Blocks the current thread until all operations in the command queues on the device are completed. 
            void WaitIdle();
            void Clear();

            void FrameBegin();
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

            std::unique_ptr<vulkan::Base> m_base;
            std::unique_ptr<vulkan::DepthStencil> m_depthStencil;
            std::unique_ptr<vulkan::Swapchain> m_swapchain;
            std::unique_ptr<vulkan::Commands> m_commands;
            vulkan::Renderer* m_renderer;
            std::unique_ptr<vulkan::ResourceManager> m_resourceManager;

            Vector2 m_dimensions;
            bool m_isMinimized;
            bool m_isFullscreen;
            bool m_isResized;
            Vector3 m_cameraWorldPosition;
            DirectX::XMMATRIX m_viewMatrix;
            DirectX::XMMATRIX m_projectionMatrix;
            std::array<float, 4> m_clearColor;
            uint32_t m_drawCallCount = 0;

    };
}