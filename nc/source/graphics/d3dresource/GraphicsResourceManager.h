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

            template<std::derived_from<GraphicsResource> T, class...Params>
            static GraphicsResource* Acquire(const std::string& uid, Params&&...p)
            {
                return Get().Acquire_<T>(uid, std::forward<Params>(p)...);
            }

            template<std::derived_from<GraphicsResource> T>
            static bool Exists(const std::string& uid)
            {
                return Get().Exists_<T>(uid);
            }

            static uint32_t AssignId()
            {
                return Get().m_resourceId++;
            }

        private:
            std::unordered_map<std::string, std::unique_ptr<GraphicsResource>> m_resources;
            Graphics* m_graphics = nullptr;
            uint32_t m_resourceId;

            static GraphicsResourceManager& Get()
            {
                static GraphicsResourceManager instance;
                return instance;
            }

            template<std::derived_from<GraphicsResource> T, typename...Params>
            GraphicsResource* Acquire_(const std::string& uid, Params&&...p)
            {
                if(const auto it = m_resources.find(uid); it != m_resources.end())
                    return it->second.get();
                
                auto [it, result] = m_resources.emplace(uid, std::make_unique<T>(std::forward<Params>(p)...));
                if(!result)
                    throw std::runtime_error("GraphicsResourceManager::Acquire_ - failed to emplace ");
                
                return it->second.get();
            }

            template<std::derived_from<GraphicsResource> T>
            bool Exists_(const std::string& uid)
            {
                return m_resources.end() != m_resources.find(uid);
            }
    };
}