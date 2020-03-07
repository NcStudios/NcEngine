#include "Graphics.h"
#include <d3dcompiler.h>
//#include "DirectXMath.h"
#include "DXException.h"

//#include "imgui_impl_dx11.h"

#include <iostream>

namespace nc::graphics::internal{

Graphics::Graphics(HWND hwnd, float screenWidth, float screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hwnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    ThrowIfFailed
    (
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                                      nullptr, 0, nullptr, 0,
                                      D3D11_SDK_VERSION, &sd, &m_swap,
                                      &m_device, nullptr, &m_context),
        __FILE__, __LINE__
    );

    //rasterizer desc
    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    //rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = FALSE;
    rastDesc.DepthBias = 0;
    rastDesc.SlopeScaledDepthBias = 0.0f;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.ScissorEnable = FALSE;
    rastDesc.MultisampleEnable = FALSE;
    rastDesc.AntialiasedLineEnable = FALSE;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    m_device->CreateRasterizerState(&rastDesc, &rasterizerState);
    m_context->RSSetState(rasterizerState.Get());    


    //get back buffer
    Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer = nullptr;
    ThrowIfFailed(m_swap->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer), __FILE__, __LINE__);
    ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_target), __FILE__, __LINE__);

    //depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
    depthStencilStateDesc.DepthEnable = TRUE;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    ThrowIfFailed(m_device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState), __FILE__, __LINE__);
    m_context->OMSetDepthStencilState(depthStencilState.Get(), 1u);

    //depth stencil texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
    D3D11_TEXTURE2D_DESC depthTextDesc = {};
    depthTextDesc.Width = screenWidth;
    depthTextDesc.Height = screenHeight;
    depthTextDesc.MipLevels = 1u;
    depthTextDesc.ArraySize = 1u;
    depthTextDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthTextDesc.SampleDesc.Count = 1u;
    depthTextDesc.SampleDesc.Quality = 0u;
    depthTextDesc.Usage = D3D11_USAGE_DEFAULT;
    depthTextDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ThrowIfFailed(m_device->CreateTexture2D(&depthTextDesc, nullptr, &depthStencilTexture), __FILE__, __LINE__);

    //create depth stencil texture view
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0u;
    ThrowIfFailed(m_device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, &m_dsv), __FILE__, __LINE__);

    //bind depth stencil view
    m_context->OMSetRenderTargets(1u, m_target.GetAddressOf(), m_dsv.Get());

    //Viewport configuration
    D3D11_VIEWPORT viewport;
    viewport.Width = m_screenWidth;
    viewport.Height = m_screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    m_context->RSSetViewports(1u, &viewport);


    

    //imgui dx bindings
    //ImGui_ImplDX11_Init(m_device.Get(), m_context.Get()); //shutdown in destr?
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
    return m_camera;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
    return m_projection;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cam) noexcept
{
    m_camera = cam;
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
    m_projection = proj;
}

void Graphics::DrawIndexed(UINT count)
{
    //m_context->Draw(count, 0u);
    m_context->DrawIndexed(count, 0u, 0u);
}

void Graphics::EndFrame()
{
    ThrowIfFailed(m_swap->Present(1u, 0u), __FILE__, __LINE__);
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
    const float color[] = {red, green, blue, 1.0f};
    m_context->ClearRenderTargetView(m_target.Get(), color);
    m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

} // end namespace nc::internal

