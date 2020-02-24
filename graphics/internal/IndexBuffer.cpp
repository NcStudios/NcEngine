#include "IndexBuffer.h"
#include "DXException.h"

namespace nc::graphics::internal
{
    IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<uint16_t>& indices)
        : count((UINT)indices.size())
    {
        D3D11_BUFFER_DESC ibd = {};
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.CPUAccessFlags = 0u;
        ibd.MiscFlags = 0u;
        ibd.ByteWidth = UINT(count * sizeof(uint16_t));
        ibd.StructureByteStride = sizeof(uint16_t);
        D3D11_SUBRESOURCE_DATA isd = {};
        isd.pSysMem = indices.data();
        ThrowIfFailed
        (
            GetDevice(graphics)->CreateBuffer(&ibd, &isd, &m_indexBuffer)
        );
    }

    void IndexBuffer::Bind(Graphics& graphics) noexcept
    {
        GetContext(graphics)->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
    }

    UINT IndexBuffer::GetCount() const noexcept
    {
        return count;
    }

} //end namespace nc::graphics::internal