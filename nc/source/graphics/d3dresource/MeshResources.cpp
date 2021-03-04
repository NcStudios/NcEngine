#include "MeshResources.h"
    
namespace nc::graphics::d3dresource
{
    /*****************
     * Vertex Buffer *
     *****************/
    VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices)
        : m_stride(sizeof(Vertex)),
          m_vertexBuffer{nullptr}
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

    std::string VertexBuffer::GetUID(const std::string& tag) noexcept
    {
        return typeid(VertexBuffer).name() + tag;
    }

    /****************
     * Index Buffer *
     ****************/
    IndexBuffer::IndexBuffer(const std::vector<uint16_t>& indices)
        : m_count((UINT)indices.size()),
          m_indexBuffer{nullptr}
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

    std::string IndexBuffer::GetUID(const std::string& tag) noexcept
    {
        return typeid(IndexBuffer).name() + tag;
    }

    /****************
     * Input Layout *
     ****************/
    InputLayout::InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
                             ID3DBlob* vertexShaderBytecode )
    {
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
    
    std::string InputLayout::GetUID(const std::string& tag) noexcept
    {
        return typeid(InputLayout).name() + tag;
    }

    /**********************
     * Primitive Topology *
     **********************/
    Topology::Topology(D3D11_PRIMITIVE_TOPOLOGY type)
        : m_type(type)
    {
    }

    void Topology::Bind() noexcept
    {
        GetContext()->IASetPrimitiveTopology(m_type);
    }
    std::string Topology::GetUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept
    {
        return typeid(Topology).name() + std::to_string(topology);
    }
}