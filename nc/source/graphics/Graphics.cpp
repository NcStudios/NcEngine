#include "Graphics.h"
#include "DXException.h"
#include "d3dresource/GraphicsResourceManager.h"

namespace
{
    constexpr float CLEAR_COLOR[] = {0.2f, 0.2f, 0.2f, 1.0f};
}

namespace nc::graphics
{
Graphics::Graphics(HWND hwnd, float width, float height, float nearZ, float farZ, bool fullscreen)
    : m_device{ nullptr },
      m_context{ nullptr },
      m_swapChain{ nullptr },
      m_renderTarget{ nullptr },
      m_dsv{ nullptr },
      m_isFullscreen {fullscreen},
      m_camera{},
      m_projection{}
{  
    d3dresource::GraphicsResourceManager::SetGraphics(this);
    CreateDeviceAndSwapchain(hwnd);
    CreateRasterizerState();
    CreateRenderTargetViewFromBackBuffer();
    CreateDepthStencilView(width, height);
    BindDepthStencilView();
    ConfigureViewport(width, height);
    SetProjection(width, height, nearZ, farZ);

    m_swapChain->SetFullscreenState(m_isFullscreen, nullptr);
}

Graphics::~Graphics() 
{
    m_swapChain->SetFullscreenState(FALSE, nullptr); // D3D can't close in fullscreen mode. switch to windowed mode on attempting to close
    m_renderTarget->Release();
    m_dsv->Release();
    m_swapChain->Release();
    m_context->Release();
    m_device->Release();
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

void Graphics::SetProjection(float width, float height, float nearZ, float farZ) noexcept
{
    m_projection = DirectX::XMMatrixPerspectiveLH(1.0f, height / width, nearZ, farZ);
}

void Graphics::ToggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    m_swapChain->SetFullscreenState(m_isFullscreen, nullptr);
}

void Graphics::ResizeTarget(float width, float height)
{
    auto mode = DXGI_MODE_DESC
    {
        (UINT)width, (UINT)height, {0, 0},
        DXGI_FORMAT_UNKNOWN,
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
        DXGI_MODE_SCALING_UNSPECIFIED
    };

    m_swapChain->ResizeTarget(&mode);
}

void Graphics::OnResize(float width, float height, float nearZ, float farZ)
{    
    m_context->OMSetRenderTargets(0,0,0);
    m_renderTarget->Release();
    m_dsv->Release();
    THROW_FAILED(m_swapChain->ResizeBuffers(0u, 0u, 0u, DXGI_FORMAT_UNKNOWN, 0u));

    CreateRenderTargetViewFromBackBuffer();
    CreateDepthStencilView(width, height);
    BindDepthStencilView();
    ConfigureViewport(width, height);
    SetProjection(width, height, nearZ, farZ);
}

void Graphics::FrameBegin()
{
    #ifdef NC_EDITOR_ENABLED
    m_drawCallCount = 0u;
    #endif
    m_context->ClearRenderTargetView(m_renderTarget, CLEAR_COLOR);
    m_context->ClearDepthStencilView(m_dsv, D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawIndexed(UINT count)
{
    #ifdef NC_EDITOR_ENABLED
    ++m_drawCallCount;
    #endif
    m_context->DrawIndexed(count, 0u, 0u);
}

void Graphics::FrameEnd()
{
    THROW_FAILED(m_swapChain->Present(1u, 0u));
}

void Graphics::CreateDeviceAndSwapchain(HWND hwnd)
{
    //d3d core init
    auto sd = DXGI_SWAP_CHAIN_DESC
    {
        //struct BufferDesc
        { 0, 0,                                //Width, Height, 
        { 0, 0 },                              //RefreshRate {Num,Den}
        DXGI_FORMAT_B8G8R8A8_UNORM,            //Format
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,  //ScanlineOrdering
        DXGI_MODE_SCALING_UNSPECIFIED },       //Scaling
        { 4, 0 },                              //struct SampleDesc {Count,Quality}
        DXGI_USAGE_RENDER_TARGET_OUTPUT, 2,    //BufferUsage, BufferCount
        hwnd, TRUE, DXGI_SWAP_EFFECT_DISCARD,  //OutputWindow, Windowed, SwapEffect,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH //Flags (Allows full-screen mode)
    };
    THROW_FAILED
    (
        D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
                                      nullptr, 0, nullptr, 0,
                                      D3D11_SDK_VERSION, &sd, &m_swapChain,
                                      &m_device, nullptr, &m_context)
    );
}

void Graphics::CreateRasterizerState()
{
    ID3D11RasterizerState* rasterizerState = nullptr;
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
        TRUE,            //MultisampleEnable
        FALSE             //AntialiasedLineEnable
    };
    m_device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    m_context->RSSetState(rasterizerState);

    rasterizerState->Release();
}

void Graphics::CreateRenderTargetViewFromBackBuffer()
{
    ID3D11Resource* backBuffer = nullptr;
    THROW_FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer)));
    THROW_FAILED(m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTarget));
    backBuffer->Release();
}

void Graphics::CreateDepthStencilView(float width, float height) 
{
    //depth stencil state
    ID3D11DepthStencilState*  depthStencilState;
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
    depthStencilStateDesc.DepthEnable              = TRUE;
    depthStencilStateDesc.DepthWriteMask           = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc                = D3D11_COMPARISON_LESS;
    THROW_FAILED(m_device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState));
    m_context->OMSetDepthStencilState(depthStencilState, 1u);
    depthStencilState->Release();

    //depth stencil texture
    ID3D11Texture2D* depthStencilTexture;
    auto depthTextDesc = D3D11_TEXTURE2D_DESC
    {
        (unsigned int)width,      //Width
        (unsigned int)height,     //Height
        1u, 1u,                   //MipLevels, ArraySize
        DXGI_FORMAT_D32_FLOAT,    //Format
        { 4u, 0u },               //SampleDesc {Count, Quality}
        D3D11_USAGE_DEFAULT,      //Usage
        D3D11_BIND_DEPTH_STENCIL, //BindFlags
        0u, 0u                    //CPUFlags, MiscFlags
    };
    THROW_FAILED(m_device->CreateTexture2D(&depthTextDesc, nullptr, &depthStencilTexture));

    //create depth stencil texture view
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format                        = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension                 = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    dsvDesc.Texture2D.MipSlice            = 0u;
    THROW_FAILED(m_device->CreateDepthStencilView(depthStencilTexture, &dsvDesc, &m_dsv));
    depthStencilTexture->Release();
}

void Graphics::BindDepthStencilView()
{
    m_context->OMSetRenderTargets(1u, &m_renderTarget, m_dsv);
}

void Graphics::ConfigureViewport(float width, float height)
{
    auto viewport = D3D11_VIEWPORT
    {
        0.0f,  0.0f,   //TopLeftX, TopLeftY
        width, height, //Width, Height
        0.0f,  1.0f    //MinDepth, MaxDepth
    };
    m_context->RSSetViewports(1u, &viewport);
}

#ifdef NC_EDITOR_ENABLED
uint32_t Graphics::GetDrawCallCount() const
{
    return m_drawCallCount;
}
#endif

} // end namespace nc::graphics

