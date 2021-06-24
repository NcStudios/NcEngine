#pragma once
#include "platform/win32/NCWin32.h"
#include "math/Vector.h"

#include <array>
#include <d3d11.h>
#include <stdint.h>
#include "directx/math/DirectXMath.h"

namespace nc
{
    namespace ui { class UIImpl; }

    namespace graphics
    {
        namespace d3dresource { class GraphicsResource; }

        class Graphics
        {
            friend graphics::d3dresource::GraphicsResource;
            friend ::nc::ui::UIImpl;

            public:
                Graphics(HWND hwnd, Vector2 dimensions);
                ~Graphics() noexcept;
                Graphics(const Graphics&) = delete;
                Graphics(Graphics&&) = delete;
                Graphics& operator=(const Graphics&) = delete;
                Graphics& operator=(Graphics&&) = delete;

                DirectX::FXMMATRIX GetViewMatrix() const noexcept;
                DirectX::FXMMATRIX GetProjectionMatrix() const noexcept;

                void SetViewMatrix(DirectX::FXMMATRIX cam) noexcept;
                void SetProjectionMatrix(float width, float height, float nearZ, float farZ) noexcept;

                void ResizeTarget(float width, float height);
                void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);
                void ToggleFullscreen();
                void SetClearColor(std::array<float, 4> color);
                
                void FrameBegin();
                void DrawIndexed(UINT count);
                void FrameEnd();

                #ifdef NC_EDITOR_ENABLED
                uint32_t GetDrawCallCount() const;
                #endif

            private:
                ID3D11Device* m_device;
                ID3D11DeviceContext* m_context;
                IDXGISwapChain* m_swapChain;
                ID3D11RenderTargetView* m_renderTarget;
                ID3D11DepthStencilView* m_dsv;
                std::array<float, 4> m_clearColor;
                bool m_isFullscreen;
                DirectX::XMMATRIX m_viewMatrix;
                DirectX::XMMATRIX m_projectionMatrix;

                #ifdef NC_EDITOR_ENABLED
                uint32_t m_drawCallCount = 0;
                #endif

                void CreateDeviceAndSwapchain(HWND hwnd);
                void CreateRenderTargetViewFromBackBuffer();
                void CreateDepthStencilView(float width, float height);
                void BindDepthStencilView();
                void ConfigureViewport(float width, float height);
        };
    } //end namespace graphics
} //end namespace nc


