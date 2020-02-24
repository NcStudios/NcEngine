#include "TransformCbuf.h"

namespace nc::graphics::internal
{
    TransformCbuf::TransformCbuf(Graphics& graphics, const Drawable& parent)
        : m_parent( parent )
    {
        if(!m_vcbuf)
        {
            m_vcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(graphics);
        }
    }

    void TransformCbuf::Bind(Graphics& graphics) noexcept
    {
        m_vcbuf->Update(graphics,
                     DirectX::XMMatrixTranspose(
                     m_parent.GetTransformXM() * graphics.GetProjection())
        );
        
        m_vcbuf->Bind(graphics);
    }

    std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::m_vcbuf;
}