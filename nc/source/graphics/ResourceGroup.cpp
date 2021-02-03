#include "graphics/ResourceGroup.h"
#include "d3dresource/GraphicsResource.h"

namespace nc::graphics
{
    void ResourceGroup::Bind() const
    {
        for(auto& res : m_resources)
        {
            res->Bind();
        }
    }

    void ResourceGroup::AddGraphicsResource(d3dresource::GraphicsResource* res)
    {
        m_resources.push_back(res);
    }
}