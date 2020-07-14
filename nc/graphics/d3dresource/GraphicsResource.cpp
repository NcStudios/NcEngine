#include "GraphicsResource.h"
#include "../Graphics.h"
#include <d3dcompiler.h>
#include "directx/math/DirectXMath.h"
#include "Transform.h"
#include "graphics/WICTextureLoader.h"
#include <string>

namespace nc::graphics::d3dresource
{
    ID3D11DeviceContext* GraphicsResource::GetContext() noexcept
    {
        return GraphicsResourceManager::GetGraphics()->m_context.Get();
    }

    ID3D11Device* GraphicsResource::GetDevice() noexcept
    {
        return GraphicsResourceManager::GetGraphics()->m_device.Get();
    }

    Sampler::Sampler(const std::string& tag)
        : m_tag(tag)
    {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

        THROW_FAILED
        (
            GetDevice()->CreateSamplerState(&samplerDesc, &m_sampler),
            __FILE__, __LINE__
        );
    }

    void Sampler::Bind() noexcept
    {
        GetContext()->PSSetSamplers(0, 1, m_sampler.GetAddressOf());
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
        THROW_FAILED
        (
            GetDevice()->CreateBuffer(&bd,&sd,&m_vertexBuffer),
            __FILE__, __LINE__
        );
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

        THROW_FAILED
        (
            GetDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer),
            __FILE__, __LINE__
        );
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

        THROW_FAILED(CreateWICTextureFromFile(GetDevice(), GetContext(), w_path.c_str(), &m_texture, &m_textureView, 0), __FILE__, __LINE__);
    }

    uint32_t Texture::GetShaderIndex() 
    {
        return m_shaderIndex;
    }

    void Texture::Bind() noexcept
    {
        GetContext()->PSSetShaderResources(m_shaderIndex, 1u, m_textureView.GetAddressOf());
    }

    VertexShader::VertexShader(const std::string& path)
        : m_path(path)
    {
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        THROW_FAILED(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob), __FILE__, __LINE__);
        
        THROW_FAILED
        (
            GetDevice()->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(),
                                                    m_bytecodeBlob->GetBufferSize(),
                                                    nullptr, &m_vertexShader),
            __FILE__, __LINE__
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
        THROW_FAILED(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob), __FILE__, __LINE__);

        // Create the shader
        THROW_FAILED
        (
            GetDevice()->CreatePixelShader( m_bytecodeBlob->GetBufferPointer(),
                                            m_bytecodeBlob->GetBufferSize(),
                                            nullptr, &m_pixelShader),
            __FILE__, __LINE__
        );
    }

    void PixelShader::Bind() noexcept
    {
        GetContext()->PSSetShader(m_pixelShader.Get(),nullptr,0u);
    }

    TransformConstBuffer::TransformConstBuffer(const std::string& tag, Model & parent, UINT slot)
        : m_parent( parent )
    {
        (void)tag;
        if(!m_vcbuf)
        {
            m_vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(slot);
        }
    }

    void TransformConstBuffer::Bind() noexcept
    {
        const auto gfx = GraphicsResourceManager::GetGraphics();
        const auto modelView = m_parent.GetTransformXM() * gfx->GetCamera();

        const Transforms t = 
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * gfx->GetProjection())
        };

        m_vcbuf->Update(t);
        m_vcbuf->Bind();
    }

    std::unique_ptr<VertexConstantBuffer<TransformConstBuffer::Transforms>> TransformConstBuffer::m_vcbuf;




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
                                                   &m_inputLayout),
            __FILE__, __LINE__
        );
    }

    void InputLayout::Bind() noexcept
    {
        GetContext()->IASetInputLayout(m_inputLayout.Get());
    }



    Topology::Topology( D3D11_PRIMITIVE_TOPOLOGY type)
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
    std::string Sampler::GetUID(const std::string& tag) noexcept
    {
        return typeid(Sampler).name() + tag;
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

    std::string TransformConstBuffer::GetUID(const std::string& tag) noexcept
    {
        return typeid(TransformConstBuffer).name() + tag;
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
        return typeid(Texture).name() + path;
    }
}