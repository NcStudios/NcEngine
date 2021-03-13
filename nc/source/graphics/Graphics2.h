#pragma once

#include "math/Vector2.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "platform/win32/NcWin32.h"
#include "directx/math/DirectXMath.h"
#include <memory>

namespace nc::graphics
{
    namespace vulkan { class Instance; class Base; class GraphicsPipeline; class RenderPass; class FrameBuffers; class Commands; class VertexBuffer; class IndexBuffer; }

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

            void ResizeTarget(float width, float height);
            void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);
            void ToggleFullscreen();

            // Blocks the current thread until all operations in the command queues on the device are completed. 
            void WaitIdle();

            void FrameBegin();
            void Draw();
            void FrameEnd();

    #ifdef NC_EDITOR_ENABLED
            uint32_t GetDrawCallCount() const;
    #endif

        private:
            void RecreateSwapChain(Vector2 dimensions);
            bool GetNextImageIndex(uint32_t& imageIndex);
            void RenderToImage(uint32_t imageIndex);
            bool PresentImage(uint32_t imageIndex);

            std::unique_ptr<vulkan::Instance> m_instance;
            std::unique_ptr<vulkan::Base> m_base;
            std::unique_ptr<vulkan::RenderPass> m_renderPass; // @todo: make vector, map, etc depending on chosen architecture. Right now just a single render pass for rendering a triangle.
            std::unique_ptr<vulkan::GraphicsPipeline> m_pipeline;
            std::unique_ptr<vulkan::FrameBuffers> m_frameBuffers;
            std::unique_ptr<vulkan::Commands> m_commands;
            std::unique_ptr<vulkan::VertexBuffer> m_vertexBuffer; // @todo: Take from mesh, will not be a member of Graphics2
            std::unique_ptr<vulkan::IndexBuffer> m_indexBuffer; // @todo: Take from mesh, will not be a member of Graphics2

            Vector2 m_dimensions;
            bool m_isMinimized;
            bool m_isFullscreen;
            DirectX::XMMATRIX m_viewMatrix;
            DirectX::XMMATRIX m_projectionMatrix;
    };
}