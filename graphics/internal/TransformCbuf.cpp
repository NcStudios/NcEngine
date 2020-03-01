#include "TransformCbuf.h"
#include "Graphics.h"

namespace nc::graphics::internal
{
    TransformCbuf::TransformCbuf(Graphics& graphics, const Drawable& parent)
        : m_parent( parent )
    {
        if(!m_vcbuf)
        {
            m_vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(graphics);
            //m_vcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(graphics);
        }
    }

    void TransformCbuf::Bind(Graphics& graphics) noexcept
    {
        const auto model = m_parent.GetTransformXM();
        const Transforms t = 
        {
            DirectX::XMMatrixTranspose(model),
            DirectX::XMMatrixTranspose(
                model *
                graphics.GetCamera() *
                graphics.GetProjection()
            )
        };

        m_vcbuf->Update(graphics, t);

        // m_vcbuf->Update(graphics,
        //                 DirectX::XMMatrixTranspose(
        //                     m_parent.GetTransformXM() *
        //                     graphics.GetCamera()   *
        //                     graphics.GetProjection())
        //                 );
        
        m_vcbuf->Bind(graphics);
    }

    std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::m_vcbuf;
    //std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::m_vcbuf;
}