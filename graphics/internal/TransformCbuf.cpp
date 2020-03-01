#include "TransformCbuf.h"
#include "Graphics.h"

namespace nc::graphics::internal
{
    TransformCbuf::TransformCbuf(Graphics& graphics, const Drawable& parent, UINT slot)
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
}