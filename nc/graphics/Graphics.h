#pragma once
#include "NCWin32.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <stdint.h>
#include "DirectXMath.h"

namespace nc::utils::editor         { class EditorManager;    }
namespace nc::graphics::d3dresource { class GraphicsResource; }

namespace nc::graphics
{
    class Graphics
    {
        friend graphics::d3dresource::GraphicsResource;
        friend nc::utils::editor::EditorManager;

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

            void DrawIndexed(UINT count);
            void EndFrame();
            void ClearBuffer(float red, float green, float blue);

        private:
            float m_screenWidth, m_screenHeight;
            DirectX::XMMATRIX m_camera;
            DirectX::XMMATRIX m_projection;
            Microsoft::WRL::ComPtr<ID3D11Device>           m_device  = nullptr;
            Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swap    = nullptr;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context = nullptr;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_target  = nullptr;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv     = nullptr;
    };  
}