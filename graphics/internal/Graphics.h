#pragma once
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <stdint.h>
#include "DirectXMath/Inc/DirectXMath.h"

namespace nc::graphics::internal
{
    class Bindable;

    class Graphics
    {
        friend Bindable;

        public:
            Graphics(HWND hwnd, float screenWidth, float screenHeight);
            ~Graphics() = default;
            Graphics(const Graphics&) = delete;
            Graphics(Graphics&&) = delete;
            Graphics& operator=(const Graphics&) = delete;
            Graphics& operator=(Graphics&&) = delete;

            //void DrawTriangle(float angle, float mouseX, float mouseY);
            void DrawIndexed(UINT count);
            
            void EndFrame();
            void ClearBuffer(float red, float green, float blue);

            void SetProjection(DirectX::FXMMATRIX proj) noexcept;
            DirectX::XMMATRIX GetProjection() const noexcept;

        private:
            float m_screenWidth, m_screenHeight;
            DirectX::XMMATRIX m_projection;
            Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
            Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap = nullptr;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_target = nullptr;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv = nullptr;
    };  
}