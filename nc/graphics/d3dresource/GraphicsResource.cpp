#include "GraphicsResource.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include "DirectXMath.h"

namespace nc::graphics::d3dresource
{
    ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& graphics) noexcept
    {
        return graphics.m_context.Get();
    }

    ID3D11Device* GraphicsResource::GetDevice(Graphics& graphics) noexcept
    {
        return graphics.m_device.Get();
    }


    VertexBuffer::VertexBuffer(Graphics& graphics,const std::vector<Vertex>& vertices, const std::string& tag)
        : m_stride(sizeof(Vertex)), m_tag(tag)
    {
        D3D11_BUFFER_DESC bd      = {};
        bd.BindFlags              = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage                  = D3D11_USAGE_DEFAULT;
        bd.CPUAccessFlags         = 0u;
        bd.MiscFlags              = 0u;
        bd.ByteWidth              = UINT(m_stride * vertices.size());
        bd.StructureByteStride    = m_stride;

        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem                = vertices.data();

        ThrowIfFailed
        (
            GetDevice(graphics)->CreateBuffer(&bd,&sd,&m_vertexBuffer),
            __FILE__, __LINE__
        );
    }


    void VertexBuffer::Bind(Graphics& graphics) noexcept
    {
        const UINT offset = 0u;
        GetContext(graphics)->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &offset);
    }



    IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<uint16_t>& indices, std::string& tag)
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

        ThrowIfFailed
        (
            GetDevice(graphics)->CreateBuffer(&ibd, &isd, &m_indexBuffer),
            __FILE__, __LINE__
        );
    }

    void IndexBuffer::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
    }

    UINT IndexBuffer::GetCount() const noexcept
    {
        return m_count;
    }




    VertexShader::VertexShader(Graphics& graphics, const std::string& path)
        : m_path(path)
    {
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        ThrowIfFailed(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob), __FILE__, __LINE__);
        
        ThrowIfFailed
        (
            GetDevice(graphics)->CreateVertexShader(m_bytecodeBlob->GetBufferPointer(),
                                                    m_bytecodeBlob->GetBufferSize(),
                                                    nullptr, &m_vertexShader),
            __FILE__, __LINE__
        );
    }

    void VertexShader::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->VSSetShader(m_vertexShader.Get(),nullptr,0u);
    }

    ID3DBlob* VertexShader::GetBytecode() const noexcept
    {
        return m_bytecodeBlob.Get();
    }



    PixelShader::PixelShader( Graphics& graphics,const std::string& path )
        : m_path(path)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
        std::wstring w_path;
        w_path.assign(path.begin(), path.end());
        ThrowIfFailed(D3DReadFileToBlob(w_path.c_str(),&m_bytecodeBlob), __FILE__, __LINE__);
        ThrowIfFailed
        (
            GetDevice(graphics)->CreatePixelShader( m_bytecodeBlob->GetBufferPointer(),
                                                    m_bytecodeBlob->GetBufferSize(),
                                                    nullptr, &m_pixelShader),
            __FILE__, __LINE__
        );
    }

    void PixelShader::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->PSSetShader(m_pixelShader.Get(),nullptr,0u);
    }



    TransformCbuf::TransformCbuf(Graphics& graphics, const std::string& tag, const Model& parent, UINT slot)
        : m_parent( parent )
    {
        if(!m_vcbuf)
        {
            m_vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(graphics, slot);
        }
    }

    void TransformCbuf::Bind(Graphics& graphics) noexcept
    {
        const auto modelView = m_parent.GetTransformXM() * graphics.GetCamera();
        const Transforms t = 
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * graphics.GetProjection())
        };

        m_vcbuf->Update(graphics, t);
        m_vcbuf->Bind(graphics);
    }

    std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::m_vcbuf;




    InputLayout::InputLayout(Graphics& graphics,
                             const std::string& tag,
                             const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                             ID3DBlob* vertexShaderBytecode )
    {

        ThrowIfFailed
        (
            GetDevice(graphics)->CreateInputLayout(layout.data(),
                                                   (UINT)layout.size(),
                                                   vertexShaderBytecode->GetBufferPointer(),
                                                   vertexShaderBytecode->GetBufferSize(),
                                                   &m_inputLayout),
            __FILE__, __LINE__
        );
    }

    void InputLayout::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->IASetInputLayout(m_inputLayout.Get());
    }



    Topology::Topology(Graphics& graphics,D3D11_PRIMITIVE_TOPOLOGY type) : m_type(type) {}

    void Topology::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->IASetPrimitiveTopology(m_type);
    }



    /****************
    * UID Functions *
    ****************/
    std::string VertexBuffer::GetUID(const std::vector<Vertex>& vertices, const std::string& tag) noexcept
    {
        return typeid(VertexBuffer).name() + tag;
    }

    std::string IndexBuffer::GetUID(const std::vector<uint16_t>& indices, std::string& tag) noexcept
    {
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

    std::string TransformCbuf::GetUID(const std::string& tag, const Model& parent, UINT slot) noexcept
    {
        return typeid(TransformCbuf).name() + tag;
    }

    std::string InputLayout::GetUID(const std::string& tag, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* vertexShaderByteCode) noexcept
    {
        return typeid(InputLayout).name() + tag;
    }

    std::string Topology::GetUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept
    {
        return typeid(Topology).name() + std::to_string(topology);
    }
}