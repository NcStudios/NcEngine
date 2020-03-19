#pragma once
#include <memory>
#include <unordered_map>
#include "GraphicsResource.h"


#include <iostream>

namespace nc::graphics { class Graphics; }

namespace nc::graphics::d3dresource
{
    class GraphicsResource;
    
    class GraphicsResourceManager
    {
        public:
            template<class T, typename...Params>
            static std::shared_ptr<GraphicsResource> Aquire(Graphics& graphics, Params&&...p)
            {
                return Get().Aquire_<T>(graphics, std::forward<Params>(p)...);
            }
        
        private:
            std::unordered_map<std::string, std::shared_ptr<GraphicsResource>> m_resources;

            static GraphicsResourceManager& Get()
            {
                static GraphicsResourceManager instance;
                return instance;
            }

            template<class T, typename...Params>
            std::shared_ptr<GraphicsResource> Aquire_(Graphics& graphics, Params&&...p)
            {
                const auto key = T::GetUID(std::forward<Params>(p)...);
                const auto i = m_resources.find(key);
                if(i == m_resources.end())
                {
                    auto res = std::make_shared<T>(graphics, std::forward<Params>(p)...);
                    m_resources[key] = res;
                    return res;
                }
                return i->second;
            }
    };
}