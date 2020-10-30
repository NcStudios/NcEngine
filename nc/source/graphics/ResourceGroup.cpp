#include "ResourceGroup.h"
#include "d3dresource/GraphicsResource.h"

namespace nc::graphics
{
    void ResourceGroup::BindGraphicsResources() const
    {
        for(auto& res : m_resources)
        {
            res->Bind();
        }
    }

    void ResourceGroup::AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res)
    {
        m_resources.push_back(std::move(res));
    }
}