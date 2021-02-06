#include "ConstantBufferResources.h"
#include "graphics/Model.h"
#include "graphics/Graphics.h"

namespace nc::graphics::d3dresource
{
    /*****************************
     * Transform Constant Buffer *
     *****************************/
    std::unique_ptr<VertexConstantBuffer<TransformConstBufferVertexPixel::Transforms>> TransformConstBufferVertexPixel::m_vcbuf = nullptr;
    std::unique_ptr<PixelConstantBuffer<TransformConstBufferVertexPixel::Transforms>> TransformConstBufferVertexPixel::m_pcbuf = nullptr;

    TransformConstBufferVertexPixel::Transforms TransformConstBufferVertexPixel::GetTransforms() noexcept
    {
        const auto gfx = GraphicsResourceManager::GetGraphics();
        const auto modelView = m_parent.GetTransformationMatrix() * gfx->GetViewMatrix();

        return
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * gfx->GetProjectionMatrix())
        };
    }

    TransformConstBufferVertexPixel::TransformConstBufferVertexPixel(const std::string& tag, Model & parent, UINT slotVertex, UINT slotPixel)
        : m_parent( parent )
    {
        (void)tag;
        if(!m_pcbuf)
        {
            m_pcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(slotPixel);
        }

        if(!m_vcbuf)
        {
            m_vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(slotVertex);
        }
    }

    void TransformConstBufferVertexPixel::Bind() noexcept
    {
        const auto transforms = GetTransforms();
        TransformConstBufferVertexPixel::UpdateBindImplementation(transforms);
    }

    void TransformConstBufferVertexPixel::UpdateBindImplementation(const Transforms& transforms) noexcept
    {
        m_vcbuf->Update(transforms);
        m_vcbuf->Bind();
        m_pcbuf->Update(transforms);
        m_pcbuf->Bind();
    }

    std::string TransformConstBufferVertexPixel::GetUID(const std::string& tag) noexcept
    {
        return typeid(TransformConstBufferVertexPixel).name() + tag;
    }
}