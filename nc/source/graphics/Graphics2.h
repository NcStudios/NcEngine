#pragma once

#include "math/Vector2.h"

#include "platform/win32/NcWin32.h"
#include "directx/math/DirectXMath.h"
#include <memory>

namespace nc::graphics
{
    namespace vulkan { class Base; class FrameBuffers; class Commands; class VertexBuffer; class IndexBuffer; class Swapchain; class DepthStencil; class FrameManager; }

    class Graphics2
    {
        public:
            Graphics2(HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);
            ~Graphics2() noexcept;
            Graphics2(const Graphics2&) = delete;
            Graphics2(Graphics2&&) = delete;
            Graphics2& operator=(const Graphics2&) = delete;
            Graphics2& operator=(Graphics2&&) = delete;

            DirectX::FXMMATRIX GetViewMatrix() const noexcept;
            DirectX::FXMMATRIX GetProjectionMatrix() const noexcept;

            void SetViewMatrix(DirectX::FXMMATRIX cam) noexcept;
            void SetProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept;
            void SetFrameManager(vulkan::FrameManager* frameManager);

            void ResizeTarget(float width, float height);
            void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);
            void ToggleFullscreen();

            const vulkan::Base& GetBase() const noexcept;
            vulkan::Base* GetBasePtr() const noexcept;
            const vulkan::Swapchain& GetSwapchain() const noexcept;
            vulkan::Commands* GetCommandsPtr() const noexcept;
            const Vector2 GetDimensions() const noexcept;

            // Blocks the current thread until all operations in the command queues on the device are completed. 
            void WaitIdle();

            void FrameBegin();
            void Draw();
            void FrameEnd();

    #ifdef NC_EDITOR_ENABLED
            uint32_t GetDrawCallCount() const;
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
            vulkan::FrameManager* m_frameManager;
            Vector2 m_dimensions;
            bool m_isMinimized;
            bool m_isFullscreen;
            bool m_isResized;
            DirectX::XMMATRIX m_viewMatrix;
            DirectX::XMMATRIX m_projectionMatrix;

    };
}