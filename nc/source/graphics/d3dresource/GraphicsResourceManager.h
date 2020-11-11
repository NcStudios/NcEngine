#pragma once

#include "GraphicsResource.h"

#include <iostream>
#include <memory>
#include <unordered_map>

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc::graphics { class Graphics; }

namespace nc::graphics::d3dresource
{
    class GraphicsResource;
    
    class GraphicsResourceManager
    {
        public:
            static void SetGraphics(Graphics * gfx)
            {
                Get().m_graphics = gfx;
            }

            static Graphics * GetGraphics()
            {
                return Get().m_graphics;
            }

            template<class T, class = typename std::enable_if<std::is_base_of<GraphicsResource, T>::value>::type, typename...Params>
            static std::shared_ptr<GraphicsResource> Acquire(Params&&...p)
            {
                return Get().Acquire_<T>(std::forward<Params>(p)...);
            }

            template<class T, class = typename std::enable_if<std::is_base_of<GraphicsResource, T>::value>::type, typename...Params>
            static bool Exists(Params&&...p)
            {
                return Get().Exists_<T>(std::forward<Params>(p)...);
            }

            static uint32_t AssignId()
            {
                return Get().m_resourceId++;
            }

            static void DisplayResources(bool* open)
            {
                Get().DisplayResources_(open);
            }

        private:
            std::unordered_map<std::string, std::shared_ptr<GraphicsResource>> m_resources;
            Graphics * m_graphics = nullptr;
            uint32_t m_resourceId;

            static GraphicsResourceManager& Get()
            {
                static GraphicsResourceManager instance;
                return instance;
            }

            template<class T, class = typename std::enable_if<std::is_base_of<GraphicsResource, T>::value>::type, typename...Params>
            std::shared_ptr<GraphicsResource> Acquire_(Params&&...p)
            {
                const auto key = T::GetUID(std::forward<Params>(p)...);
                const auto i = m_resources.find(key);
                if(i == m_resources.end())
                {
                    auto res = std::make_shared<T>(std::forward<Params>(p)...);
                    m_resources[key] = res;
                    return res;
                }
                return i->second;
            }

            template<class T, class = typename std::enable_if<std::is_base_of<GraphicsResource, T>::value>::type, typename...Params>
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

            void DisplayResources_(bool* open)
            {
                if( !(*open) ) 
                    return;

                #ifdef NC_EDITOR_ENABLED
                ImGui::Begin("Graphics Resources", open, ImGuiWindowFlags_NoBackground);
                for(auto& res : m_resources)
                {
                    ImGui::Text(res.first.c_str());
                }
                ImGui::End();
                #endif
            }
    };
}