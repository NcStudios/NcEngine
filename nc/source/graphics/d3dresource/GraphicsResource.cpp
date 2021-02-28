#include "GraphicsResource.h"
#include "graphics/Graphics.h"
#include "graphics/WICTextureLoader.h"
#include "directx/math/DirectXMath.h"
#include "component/Transform.h"

#include <string>
#include <d3dcompiler.h>

namespace nc::graphics::d3dresource
{
    ID3D11DeviceContext* GraphicsResource::GetContext() noexcept
    {
        return GraphicsResourceManager::GetGraphics()->m_context;
    }

    ID3D11Device* GraphicsResource::GetDevice() noexcept
    {
        return GraphicsResourceManager::GetGraphics()->m_device;
    }

    /**********
    * Stencil *
    ***********/
    auto stencilTagFromMode(Stencil::Mode mode)
    {
        switch (mode)
        {
            case Stencil::Mode::Write:
                return std::string{"Write"};
            case Stencil::Mode::Mask:
                return std::string{"Mask"};
            case Stencil::Mode::Off:
                return std::string{"Off"};
            default:
                throw std::runtime_error("Invalid mode specified.");
        }
    };

    Stencil::Stencil(Mode mode)
    : m_mode {mode},
      m_stencil{nullptr}
    {
        D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
        
        switch (m_mode)
        {
            case Mode::Write:
            {
                dsDesc.DepthEnable = FALSE;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
                break;
            }
            case Mode::Mask:
            {
				dsDesc.DepthEnable = FALSE;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilReadMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
                break;
            }
            case Mode::Off:
            {
                break;
            }
        }
        THROW_FAILED(GetDevice()->CreateDepthStencilState(&dsDesc, &m_stencil));
    }

    void Stencil::Bind() noexcept
    {
        GetContext()->OMSetDepthStencilState(m_stencil.Get(), 1u);
    }

    std::string Stencil::GetUID(Stencil::Mode mode) noexcept
    {
        return typeid(Stencil).name() + stencilTagFromMode(mode);
    }

    /*************
     * Rasterizer
     * ***********/
    auto rasterizerTagFromMode = [](Rasterizer::Mode mode)
    {
        switch (mode)
        {
            case Rasterizer::Mode::Wireframe:
                return std::string{"Wireframe"};
            case Rasterizer::Mode::Solid:
                return std::string{"Solid"};
            default:
                throw std::runtime_error("Invalid mode specified.");
        }
    };

    Rasterizer::Rasterizer(Mode mode)
    : m_mode {mode},
      m_rasterizer{nullptr}
    {
        D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC { CD3D11_DEFAULT{} };

        switch (m_mode)
        {
            case Mode::Solid:
            {
                rasterizerDesc.FillMode = D3D11_FILL_SOLID;
                rasterizerDesc.CullMode  = D3D11_CULL_BACK;
                rasterizerDesc.FrontCounterClockwise = FALSE;
                rasterizerDesc.DepthBias = 0;
                rasterizerDesc.DepthBiasClamp = 0.0f;
                rasterizerDesc.SlopeScaledDepthBias = 0.0f;
                rasterizerDesc.DepthClipEnable = TRUE;
                rasterizerDesc.ScissorEnable = FALSE;
                rasterizerDesc.MultisampleEnable = TRUE;
                rasterizerDesc.AntialiasedLineEnable = FALSE;
                break;
            }
            case Mode::Wireframe:
            {
				rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
                rasterizerDesc.CullMode  = D3D11_CULL_BACK;
                rasterizerDesc.FrontCounterClockwise = FALSE;
                rasterizerDesc.DepthBias = 0;
                rasterizerDesc.DepthBiasClamp = 0.0f;
                rasterizerDesc.SlopeScaledDepthBias = 0.0f;
                rasterizerDesc.DepthClipEnable = TRUE;
                rasterizerDesc.ScissorEnable = FALSE;
                rasterizerDesc.MultisampleEnable = TRUE;
                rasterizerDesc.AntialiasedLineEnable = FALSE;
                break;
            }
        }
        THROW_FAILED(GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterizer));
    }

    void Rasterizer::Bind() noexcept
    {
        GetContext()->RSSetState(m_rasterizer.Get());
    }

    std::string Rasterizer::GetUID(Rasterizer::Mode mode) noexcept
    {
        return typeid(Rasterizer).name() + rasterizerTagFromMode(mode);
    }

    /**********
    * Sampler *
    ***********/
    Sampler::Sampler()
        : m_sampler{nullptr}
    {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MinLOD = 0.0f;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

        THROW_FAILED(GetDevice()->CreateSamplerState(&samplerDesc, &m_sampler));
    }

    void Sampler::Bind() noexcept
    {
        GetContext()->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
    }

    std::string Sampler::GetUID(const std::string& tag) noexcept
    {
        return typeid(Sampler).name() + tag;
    }

    /**********
    * Blender *
    ***********/
    Blender::Blender()
        : m_blender{nullptr},
          m_isBlending{false}
    {
        D3D11_BLEND_DESC blendDesc = {};
        auto& blenderRenderTarget = blendDesc.RenderTarget[0];
        if (m_isBlending)
        {
            blenderRenderTarget.BlendEnable = TRUE;
            blenderRenderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
            blenderRenderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            blenderRenderTarget.BlendOp = D3D11_BLEND_OP_ADD;
            blenderRenderTarget.SrcBlendAlpha = D3D11_BLEND_ZERO;
            blenderRenderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
            blenderRenderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blenderRenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }
        else
        {
            blenderRenderTarget.BlendEnable = FALSE;
            blenderRenderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }
        THROW_FAILED(GetDevice()->CreateBlendState(&blendDesc, &m_blender));
    }

    void Blender::Bind() noexcept
    {
        GetContext()->OMSetBlendState(m_blender.Get(), nullptr, 0xFFFFFFFFu);
    }

    std::string Blender::GetUID(const std::string& tag) noexcept
    {
        return typeid(Blender).name() + tag;
    }

    /**********
    * Texture *
    ***********/
    Texture::Texture(const std::string& path, uint32_t shaderIndex) 
        : m_textureView{nullptr},
          m_texture{nullptr},
          m_shaderIndex{shaderIndex}
    {
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());

        THROW_FAILED(CreateWICTextureFromFile(GetDevice(), GetContext(), w_path.c_str(), &m_texture, &m_textureView, 0));
    }

    uint32_t Texture::GetShaderIndex() const
    {
        return m_shaderIndex;
    }

    void Texture::Bind() noexcept
    {
        GetContext()->PSSetShaderResources(m_shaderIndex, 1u, m_textureView.GetAddressOf());
    }

    ID3D11ShaderResourceView* Texture::GetShaderResourceView()
    {
        return m_textureView.Get();
    }

    std::string Texture::GetUID(const std::string& path) noexcept
    {
        return typeid(Texture).name() + path;
    }
}