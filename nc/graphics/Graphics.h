#pragma once
#include "win32/NCWin32.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <stdint.h>
#include "directx/math/DirectXMath.h"

namespace nc::graphics::d3dresource { class GraphicsResource; }
#ifdef NC_DEBUG
namespace nc::utils::editor         { class EditorManager;    }
#endif

namespace nc::graphics
{
    class Graphics
    {
        friend graphics::d3dresource::GraphicsResource;
        #ifdef NC_DEBUG
        friend nc::utils::editor::EditorManager;
        #endif

        public:
            Graphics(HWND hwnd, float screenWidth, float screenHeight);
            ~Graphics() = default;
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

            #ifdef NC_DEBUG
            uint32_t GetDrawCallCount() const;
            #endif

        private:
            float m_screenWidth, m_screenHeight;
            DirectX::XMMATRIX m_camera;
            DirectX::XMMATRIX m_projection;
            Microsoft::WRL::ComPtr<ID3D11Device>           m_device  = nullptr;
            Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swap    = nullptr;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context = nullptr;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_target  = nullptr;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv     = nullptr;

            #ifdef NC_DEBUG
            uint32_t m_drawCallCount = 0;
            #endif
    };  
}