#include "IndexBuffer.h"
#include "DXException.h"
#include "Graphics.h"

namespace nc::graphics::d3dresource
{
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

        std::cout << "IndexBuffer::IndexBuffer()" << std::endl;
        std::cout << "indices size: " << indices.size() << std::endl;
        if(indices.data() == nullptr) { std::cout << "nullptr" << std::endl; }
        

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

} //end namespace nc::graphics::internal