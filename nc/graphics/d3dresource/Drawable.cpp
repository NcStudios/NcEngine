#include "Drawable.h"
#include "DXException.h"
#include "IndexBuffer.h"

namespace nc::graphics::d3dresource
{
    void Drawable::Draw(Graphics& graphics) const noexcept
    {
        for(auto& b : m_binds) b->Bind(graphics);
        for(auto& b : GetStaticBinds()) b->Bind(graphics);
        graphics.DrawIndexed(m_indexBuffer->GetCount());
    }

    void Drawable::AddBind(std::unique_ptr<Bindable> bind)
    {
        //should SFINAE for bind != IndexBuffer
        m_binds.push_back(std::move(bind));
    }

    void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> iBuf)
    {
        //assert can't add index buffer second time?
        m_indexBuffer = iBuf.get();
        m_binds.push_back(std::move(iBuf));
    }
}