#pragma once

#include "GraphicsResource.h"

#include <concepts>
#include <iostream>
#include <memory>
#include <unordered_map>

namespace nc::graphics { class Graphics; }

#ifdef NC_EDITOR_ENABLED
namespace nc::ui::editor::controls { void GraphicsResourcePanel(); }
#endif

namespace nc::graphics::d3dresource
{
    class GraphicsResource;
    
    class GraphicsResourceManager
    {
        #ifdef NC_EDITOR_ENABLED
        friend void ui::editor::controls::GraphicsResourcePanel();
        #endif

        public:
            static void SetGraphics(Graphics* gfx)
            {
                Get().m_graphics = gfx;
            }

            static Graphics* GetGraphics()
            {
                return Get().m_graphics;
            }

            template<std::derived_from<GraphicsResource> T, typename...Params>
            static GraphicsResource* Acquire(Params&&...p)
            {
                return Get().Acquire_<T>(std::forward<Params>(p)...);
            }

            template<std::derived_from<GraphicsResource> T, typename...Params>
            static bool Exists(Params&&...p)
            {
                return Get().Exists_<T>(std::forward<Params>(p)...);
            }

            static uint32_t AssignId()
            {
                return Get().m_resourceId++;
            }

        private:
            std::unordered_map<std::string, std::unique_ptr<GraphicsResource>> m_resources;
            Graphics * m_graphics = nullptr;
            uint32_t m_resourceId;

            static GraphicsResourceManager& Get()
            {
                static GraphicsResourceManager instance;
                return instance;
            }

            template<std::derived_from<GraphicsResource> T, typename...Params>
            GraphicsResource* Acquire_(Params&&...p)
            {
                const auto key = T::GetUID(std::forward<Params>(p)...);
                const auto i = m_resources.find(key);
                if(i == m_resources.end())
                {
                    auto [it, result] = m_resources.emplace(key, std::make_unique<T>(std::forward<Params>(p)...));
                    if(!result)
                        throw std::runtime_error("GraphicsResourceManager::Acquire_ - failed to emplace ");
                    return it->second.get();
                }
                return i->second.get();
            }

            template<std::derived_from<GraphicsResource> T, typename...Params>
            bool Exists_(Params&&...p)
            {
                const auto key = T::GetUID(std::forward<Params>(p)...);
                const auto i = m_resources.find(key);
                if(i == m_resources.end())
                {
                    return false;
                }
                return true;
            }
    };
}