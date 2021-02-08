#pragma once

#include "math/Vector2.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "platform/win32/NcWin32.h"
#include "directx/math/DirectXMath.h"
#include <memory>

namespace nc::graphics
{
    namespace vulkan { class Instance; class Device; }

    class Graphics2
    {
        public:
            Graphics2(HWND hwnd, HINSTANCE hinstance, Vector2 dimensions);
            ~Graphics2();

            DirectX::FXMMATRIX GetViewMatrix() const noexcept;
            DirectX::FXMMATRIX GetProjectionMatrix() const noexcept;

            void SetViewMatrix(DirectX::FXMMATRIX cam) noexcept;
            void SetProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept;

            void ResizeTarget(float width, float height);
            void OnResize(float width, float height, float nearZ, float farZ);
            void ToggleFullscreen();
            
            void FrameBegin();
            void DrawIndexed(UINT count);
            void FrameEnd();

            #ifdef NC_EDITOR_ENABLED
            uint32_t GetDrawCallCount() const;
            #endif
            
        private:
            std::unique_ptr<vulkan::Instance> m_instance;
            std::unique_ptr<vulkan::Device> m_device;
            bool m_isFullscreen;
            DirectX::XMMATRIX m_viewMatrix;
            DirectX::XMMATRIX m_projectionMatrix;
    };
}