#include "GraphicsResource.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
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

    /* Stencil */
    auto tagFromMode = [](Stencil::Mode mode)
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
    : m_tag {tagFromMode(mode)},
      m_mode {mode}
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
        m_stencil->Release();
    }

    Sampler::Sampler(const std::string& tag)
        : m_tag(tag)
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

    Blender::Blender(const std::string& tag)
        : m_tag{tag},
          m_blender{nullptr},
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

    VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices, const std::string& tag)
        : m_stride(sizeof(Vertex)), m_tag(tag)
    {
        auto bd = D3D11_BUFFER_DESC
        {
            UINT(m_stride * vertices.size()), //ByteWidth
            D3D11_USAGE_DEFAULT,              //Usage
            D3D11_BIND_VERTEX_BUFFER,         //BindFlags
            0u, 0u, m_stride                  //CPUAccessFlags, MiscFlags, StructureByteStride
        };
        auto sd = D3D11_SUBRESOURCE_DATA
        {
            vertices.data(), 0u, 0u //pSysMem, SysMemPitch, SysMemSlicePitch
        };
        THROW_FAILED(GetDevice()->CreateBuffer(&bd,&sd,&m_vertexBuffer));
    }


    void VertexBuffer::Bind() noexcept
    {
        const UINT offset = 0u;
        GetContext()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &offset);
    }

    IndexBuffer::IndexBuffer(const std::vector<uint16_t>& indices, std::string& tag)
        : m_count((UINT)indices.size()), m_tag(tag)
    {
        D3D11_BUFFER_DESC ibd      = {};
        ibd.BindFlags              = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage                  = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags         = 0u;
        ibd.MiscFlags              = 0u;
        ibd.ByteWidth              = UINT(m_count * sizeof(uint16_t));
        ibd.StructureByteStride    = sizeof(uint16_t);

        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem                = indices.data();

        THROW_FAILED(GetDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer));
    }

    void IndexBuffer::Bind() noexcept
    {
        GetContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
    }

    UINT IndexBuffer::GetCount() const noexcept
    {
        return m_count;
    }

    Texture::Texture(const std::string& path, uint32_t shaderIndex) 
        : m_path(path), m_shaderIndex(shaderIndex)
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

    VertexShader::VertexShader(const std::string& path)
        : m_path(path)
    {
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        THROW_FAILED(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob));
        
        THROW_FAILED
        (
            GetDevice()->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(),
                                            m_bytecodeBlob->GetBufferSize(),
                                            nullptr, &m_vertexShader)
        );
    }

    void VertexShader::Bind() noexcept
    {
        GetContext()->VSSetShader(m_vertexShader.Get(),nullptr,0u);
    }

    ID3DBlob* VertexShader::GetBytecode() const noexcept
    {
        return m_bytecodeBlob.Get();
    }

    PixelShader::PixelShader(const std::string& path )
        : m_path(path)
    {
        // Read the shader code into a blob
        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        THROW_FAILED(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob));

        // Create the shader
        THROW_FAILED
        (
            GetDevice()->CreatePixelShader( m_bytecodeBlob->GetBufferPointer(),
                                            m_bytecodeBlob->GetBufferSize(),
                                            nullptr, &m_pixelShader)
        );
    }

    void PixelShader::Bind() noexcept
    {
        GetContext()->PSSetShader(m_pixelShader.Get(),nullptr,0u);
    }

    TransformConstBufferVertex::TransformConstBufferVertex(const std::string& tag, Model & parent, UINT slot)
        : m_parent( parent )
    {
        (void)tag;
        if(!m_vcbuf)
        {
            m_vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(slot);
        }
    }

    void TransformConstBufferVertex::Bind() noexcept
    {
        UpdateBindImplementation(GetTransforms());
    }

    void TransformConstBufferVertex::UpdateBindImplementation(const Transforms& transforms) noexcept
    {
        m_vcbuf->Update(transforms);
        m_vcbuf->Bind();
    }

    TransformConstBufferVertex::Transforms TransformConstBufferVertex::GetTransforms() noexcept
    {
        const auto gfx = GraphicsResourceManager::GetGraphics();
        const auto modelView = m_parent.GetTransformXM() * gfx->GetCamera();

        return
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * gfx->GetProjection())
        };
    }

    std::unique_ptr<VertexConstantBuffer<TransformConstBufferVertex::Transforms>> TransformConstBufferVertex::m_vcbuf;

    TransformConstBufferVertexPixel::TransformConstBufferVertexPixel(const std::string& tag, Model & parent, UINT slotVertex, UINT slotPixel)
    : TransformConstBufferVertex(tag, parent, slotVertex)
    {
        (void)tag;
        if(!m_pcbuf)
        {
            m_pcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(slotPixel);
        }
    }

    void TransformConstBufferVertexPixel::Bind() noexcept
    {
        const auto transforms = GetTransforms();
        TransformConstBufferVertex::UpdateBindImplementation(transforms);
        UpdateBindImplementation(transforms);
    }

    void TransformConstBufferVertexPixel::UpdateBindImplementation(const Transforms& transforms) noexcept
    {
        m_pcbuf->Update(transforms);
        m_pcbuf->Bind();
    }

    std::unique_ptr<PixelConstantBuffer<TransformConstBufferVertex::Transforms>> TransformConstBufferVertexPixel::m_pcbuf;

    InputLayout::InputLayout(const std::string& tag,
                             const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                             ID3DBlob* vertexShaderBytecode )
    {
        (void)tag;
        
        THROW_FAILED
        (
            GetDevice()->CreateInputLayout(layout.data(),
                                                   (UINT)layout.size(),
                                                   vertexShaderBytecode->GetBufferPointer(),
                                                   vertexShaderBytecode->GetBufferSize(),
                                                   &m_inputLayout)
        );
    }

    void InputLayout::Bind() noexcept
    {
        GetContext()->IASetInputLayout(m_inputLayout.Get());
    }



    Topology::Topology(D3D11_PRIMITIVE_TOPOLOGY type)
        : m_type(type)
    {
    }

    void Topology::Bind() noexcept
    {
        GetContext()->IASetPrimitiveTopology(m_type);
    }



    /****************
    * UID Functions *
    ****************/
    std::string Stencil::GetUID(Stencil::Mode mode) noexcept
    {
        return typeid(Stencil).name() + tagFromMode(mode);
    }

    std::string Sampler::GetUID(const std::string& tag) noexcept
    {
        return typeid(Sampler).name() + tag;
    }

    std::string Blender::GetUID(const std::string& tag) noexcept
    {
        return typeid(Blender).name() + tag;
    }

    std::string VertexBuffer::GetUID(const std::vector<Vertex>& vertices, const std::string& tag) noexcept
    {
        (void)vertices;
        return typeid(VertexBuffer).name() + tag;
    }

    std::string IndexBuffer::GetUID(const std::vector<uint16_t>& indices, std::string& tag) noexcept
    {
        (void)indices;
        return typeid(IndexBuffer).name() + tag;
    }

    std::string VertexShader::GetUID(const std::string& path) noexcept
    {
        return typeid(VertexShader).name() + path;
    }

    std::string PixelShader::GetUID(const std::string& path) noexcept
    {
        return typeid(PixelShader).name() + path;
    }

    std::string TransformConstBufferVertex::GetUID(const std::string& tag) noexcept
    {
        return typeid(TransformConstBufferVertex).name() + tag;
    }

    std::string InputLayout::GetUID(const std::string& tag, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderByteCode) noexcept
    {
        (void)layout;
        (void)vertexShaderByteCode;
        return typeid(InputLayout).name() + tag;
    }

    std::string Topology::GetUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept
    {
        return typeid(Topology).name() + std::to_string(topology);
    }

    std::string Texture::GetUID(const std::string& path, uint32_t shaderIndex) noexcept
    {
        (void)shaderIndex;
        return typeid(Texture).name() + path;
    }
}