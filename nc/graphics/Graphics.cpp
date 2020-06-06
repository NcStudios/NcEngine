#include "Graphics.h"
#include <d3dcompiler.h>
#include "DXException.h"
#include <iostream>

namespace nc::graphics{

Graphics::Graphics(HWND hwnd, float screenWidth, float screenHeight)
    : m_screenWidth(screenWidth), m_screenHeight(screenHeight)
{
    //d3d core init
    auto sd = DXGI_SWAP_CHAIN_DESC
    {
        {                                         //struct BufferDesc
            0, 0, { 0, 0 },                       //Width, Height, RefreshRate {Num,Den}
            DXGI_FORMAT_B8G8R8A8_UNORM,           //Format
            DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, //ScanlineOrdering
            DXGI_MODE_SCALING_UNSPECIFIED         //Scaling
        },
        {                                         //struct SampleDesc
            1, 0                                  //Count, Quality
        }, 
        DXGI_USAGE_RENDER_TARGET_OUTPUT, 1,       //BufferUsage, BufferCount
        hwnd, TRUE, DXGI_SWAP_EFFECT_DISCARD, 0   //OutputWindow, Windowed, SwapEffect, Flags
    };
    ThrowIfFailed
    (
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                                      nullptr, 0, nullptr, 0,
                                      D3D11_SDK_VERSION, &sd, &m_swap,
                                      &m_device, nullptr, &m_context),
        __FILE__, __LINE__
    );

    //rasterizer state
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    auto rasterizerDesc = D3D11_RASTERIZER_DESC 
    {
        D3D11_FILL_SOLID, //FillMode
        D3D11_CULL_BACK,  //CullMode
        FALSE,            //FrontCounterClockwise
        0,                //DepthBias
        0.0f,             //DepthBiasClamp
        0.0f,             //SlopeScaledDepthBias
        TRUE,             //DepthClipEnable
        FALSE,            //ScissorEnable
        FALSE,            //MultisampleEnable
        FALSE             //AntialiasedLineEnable
    };
    m_device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    m_context->RSSetState(rasterizerState.Get());    

    //get back buffer & render target view
    Microsoft::WRL::ComPtr<ID3D11Resource> backBuffer = nullptr;
    ThrowIfFailed(m_swap->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer), __FILE__, __LINE__);
    ThrowIfFailed(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_target), __FILE__, __LINE__);

    //depth stencil state
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  depthStencilState;
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
    depthStencilStateDesc.DepthEnable              = TRUE;
    depthStencilStateDesc.DepthWriteMask           = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc                = D3D11_COMPARISON_LESS;
    ThrowIfFailed(m_device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState), __FILE__, __LINE__);
    m_context->OMSetDepthStencilState(depthStencilState.Get(), 1u);

    //depth stencil texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
    auto depthTextDesc = D3D11_TEXTURE2D_DESC
    {
        (unsigned int)screenWidth,  //Width
        (unsigned int)screenHeight, //Height
        1u, 1u,                     //MipLevels, ArraySize
        DXGI_FORMAT_D32_FLOAT,      //Format
        { 1u, 0u },                 //SampleDesc {Count, Quality}
        D3D11_USAGE_DEFAULT,        //Usage
        D3D11_BIND_DEPTH_STENCIL,   //BindFlags
        0u, 0u                      //CPUFlags, MiscFlags
    };
    ThrowIfFailed(m_device->CreateTexture2D(&depthTextDesc, nullptr, &depthStencilTexture), __FILE__, __LINE__);

    //create depth stencil texture view
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format                        = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension                 = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice            = 0u;
    ThrowIfFailed(m_device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, &m_dsv), __FILE__, __LINE__);

    //bind depth stencil view
    m_context->OMSetRenderTargets(1u, m_target.GetAddressOf(), m_dsv.Get());

    //Viewport configuration
    auto viewport = D3D11_VIEWPORT
    {
        0.0f,          0.0f,           //TopLeftX, TopLeftY
        m_screenWidth, m_screenHeight, //Width, Height
        0.0f,          1.0f            //MinDepth, MaxDepth
    };
    m_context->RSSetViewports(1u, &viewport);
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

void Graphics::StartFrame()
{
    #ifdef NC_DEBUG
    m_drawCallCount = 0;
    #endif
    static const float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    m_context->ClearRenderTargetView(m_target.Get(), color);
    m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawIndexed(UINT count)
{
    #ifdef NC_DEBUG
    ++m_drawCallCount;
    #endif
    m_context->DrawIndexed(count, 0u, 0u);
}

void Graphics::EndFrame()
{
    ThrowIfFailed(m_swap->Present(1u, 0u), __FILE__, __LINE__);
}

#ifdef NC_DEBUG
uint32_t Graphics::GetDrawCallCount() const
{
    return m_drawCallCount;
}
#endif

} // end namespace nc::graphics

