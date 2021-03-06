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
            static void SetGraphics(Graphics* gfx);
            static Graphics* GetGraphics();
            static uint32_t AssignId();

            /** Creates a resource of type T, forwarding Params to the constructor, and
             *  maps it to uid. Returns whether load was succesful. */
            template<std::derived_from<GraphicsResource> T, class...Params>
            static bool Load(const std::string& uid, Params&&...p);

            /** @todo Unload and maybe UnloadAll */

            /** Returns whether a resource is mapped to uid. */
            static bool IsLoaded(const std::string& uid);

            /** Returns a pointer to the resource mapped to uid, or nullptr on failure. */
            static GraphicsResource* Acquire(const std::string& uid);

            /** Returns a pointer to the resource mapped to uid. If one does not exist,
             *  one will be loaded. */
            template<std::derived_from<GraphicsResource> T, class...Params>
            static GraphicsResource* AcquireOnDemand(const std::string& uid, Params&&...p);

        private:
            std::unordered_map<std::string, std::unique_ptr<GraphicsResource>> m_resources;
            Graphics* m_graphics = nullptr;
            uint32_t m_resourceId;

            static GraphicsResourceManager& Get();

            template<std::derived_from<GraphicsResource> T, class...Params>
            bool Load_(const std::string& uid, Params&&...p);

            bool IsLoaded_(const std::string& uid);

            GraphicsResource* Acquire_(const std::string& uid);

            template<std::derived_from<GraphicsResource> T, class...Params>
            GraphicsResource* AcquireOnDemand_(const std::string& uid, Params&&...p);
    };

    inline void GraphicsResourceManager::SetGraphics(Graphics* gfx)
    {
        Get().m_graphics = gfx;
    }

    inline Graphics* GraphicsResourceManager::GetGraphics()
    {
        return Get().m_graphics;
    }

    inline uint32_t GraphicsResourceManager::AssignId()
    {
        return Get().m_resourceId++;
    }

    inline GraphicsResourceManager& GraphicsResourceManager::Get()
    {
        static GraphicsResourceManager instance;
        return instance;
    }

    template<std::derived_from<GraphicsResource> T, class...Params>
    bool GraphicsResourceManager::Load(const std::string& uid, Params&&...p)
    {
        return Get().Load_<T>(uid, std::forward<Params>(p)...);
    }

    inline bool GraphicsResourceManager::IsLoaded(const std::string& uid)
    {
        return Get().IsLoaded_(uid);
    }

    inline GraphicsResource* GraphicsResourceManager::Acquire(const std::string& uid)
    {
        return Get().Acquire_(uid);
    }

    template<std::derived_from<GraphicsResource> T, class...Params>
    GraphicsResource* GraphicsResourceManager::AcquireOnDemand(const std::string& uid, Params&&...p)
    {
        return Get().AcquireOnDemand_<T>(uid, std::forward<Params>(p)...);
    }

    template<std::derived_from<GraphicsResource> T, typename...Params>
    bool GraphicsResourceManager::Load_(const std::string& uid, Params&&...p)
    {
        if(IsLoaded(uid))
            return false;

        auto [it, result] = m_resources.emplace(uid, std::make_unique<T>(std::forward<Params>(p)...));
        return result;
    }

    inline bool GraphicsResourceManager::IsLoaded_(const std::string& uid)
    {
        return m_resources.end() != m_resources.find(uid);
    }

    inline GraphicsResource* GraphicsResourceManager::Acquire_(const std::string& uid)
    {
        if(const auto it = m_resources.find(uid); it != m_resources.end())
            return it->second.get();
        
        return nullptr;
    }

    template<std::derived_from<GraphicsResource> T, typename...Params>
    GraphicsResource* GraphicsResourceManager::AcquireOnDemand_(const std::string& uid, Params&&...p)
    {
        if(!IsLoaded_(uid))
            Load_<T>(uid, std::forward<Params>(p)...);
        
        return Acquire_(uid);
    }
}