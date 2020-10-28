#pragma once
#include "win32/NCWin32.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <stdint.h>
#include "directx/math/DirectXMath.h"

namespace nc
{
    namespace ui { class UI; }

    namespace graphics
    {
        namespace d3dresource { class GraphicsResource; }

        class Graphics
        {
            friend graphics::d3dresource::GraphicsResource;
            friend ::nc::ui::UI;

            public:
                Graphics(HWND hwnd, float screenWidth, float screenHeight);
                ~Graphics();
                Graphics(const Graphics&) = delete;
                Graphics(Graphics&&) = delete;
                Graphics& operator=(const Graphics&) = delete;
                Graphics& operator=(Graphics&&) = delete;

                DirectX::XMMATRIX GetCamera() const noexcept;
                DirectX::XMMATRIX GetProjection() const noexcept;

                void SetCamera(DirectX::FXMMATRIX cam) noexcept;
                void SetProjection(DirectX::FXMMATRIX proj) noexcept;

                void StartFrame();
                void DrawIndexed(UINT count);
                void EndFrame();

                #ifdef NC_EDITOR_ENABLED
                uint32_t GetDrawCallCount() const;
                #endif

            private:
                float m_screenWidth, m_screenHeight;
                DirectX::XMMATRIX m_camera;
                DirectX::XMMATRIX m_projection;

                #ifdef NC_EDITOR_ENABLED
                uint32_t m_drawCallCount = 0;
                #endif

                void CreateDeviceAndSwapchain(HWND hwnd);
                void CreateRasterizerState();
                void CreateRenderTargetViewFromBackBuffer();
                void CreateDepthStencilView();
                void BindDepthStencilView();
                void ConfigureViewport();

                Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
                Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
                Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
                Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTarget = nullptr;
                Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv = nullptr;
        };
    } //end namespace graphics
} //end namespace nc


