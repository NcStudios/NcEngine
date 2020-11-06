#pragma once

#include <vector>
#include <memory>

namespace nc::graphics
{
    namespace d3dresource
    {
        class GraphicsResource;
        class IndexBuffer;
    }

    class ResourceGroup 
    {
        public:
            template<class T>
            T * QueryGraphicsResource() noexcept;
            void BindGraphicsResources() const;
            void AddGraphicsResource(std::shared_ptr<d3dresource::GraphicsResource> res);

        private:
            std::vector<std::shared_ptr<d3dresource::GraphicsResource>> m_resources;
    };

    template<class T>
    T* ResourceGroup::QueryGraphicsResource() noexcept
    {
        for(auto& res : m_resources)
        {
            if(auto pt = dynamic_cast<T*>(res.get()))
            {
                return pt;
            }
        }
        return nullptr;
    }
}