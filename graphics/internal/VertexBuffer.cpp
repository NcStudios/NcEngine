#include "VertexBuffer.h"

namespace nc::graphics::internal
{
    void VertexBuffer::Bind(Graphics& graphics) noexcept
    {
        const UINT offset = 0u;
        GetContext(graphics)->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &offset);
    }
}