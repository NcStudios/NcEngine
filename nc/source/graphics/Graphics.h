#pragma once
#include "win32/NCWin32.h"
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
                Graphics(HWND hwnd, float width, float height, float nearZ, float farZ, bool fullscreen);
                ~Graphics();
                Graphics(const Graphics&) = delete;
                Graphics(Graphics&&) = delete;
                Graphics& operator=(const Graphics&) = delete;
                Graphics& operator=(Graphics&&) = delete;

                DirectX::XMMATRIX GetCamera() const noexcept;
                DirectX::XMMATRIX GetProjection() const noexcept;

                void SetCamera(DirectX::FXMMATRIX cam) noexcept;
                void SetProjection(float width, float height, float nearZ, float farZ) noexcept;

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
                ID3D11Device* m_device;
                ID3D11DeviceContext* m_context;
                IDXGISwapChain* m_swapChain;
                ID3D11RenderTargetView* m_renderTarget;
                ID3D11DepthStencilView* m_dsv;

                bool m_isFullscreen;
                DirectX::XMMATRIX m_camera;
                DirectX::XMMATRIX m_projection;

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


