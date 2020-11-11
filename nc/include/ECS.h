#pragma once

#include "ecs/Entity.h"
#include "component/PointLight.h"
#include "component/Renderer.h"
#include "component/Transform.h"
#include "NcDebug.h"

#include <string>

namespace nc
{
    namespace ecs { class EcsImpl; }

    class Ecs
    {
        template<class T>
        using is_component_t = typename std::enable_if_t<std::is_base_of_v<Component, T>>;

        public:
            static void RegisterImpl(ecs::EcsImpl* impl);

            static EntityHandle CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, std::string tag);
            static bool DestroyEntity(EntityHandle handle);
            static Entity* GetEntity(EntityHandle handle);
            static Entity* GetEntity(std::string tag);

            template<class T, class = is_component_t<T>, class ...Args>
            static T* AddComponent(EntityHandle handle, Args&& ... args);
            
            template<class T, class = is_component_t<T>>
            static bool RemoveComponent(EntityHandle handle);
            
            template<class T, class = is_component_t<T>>
            static T* GetComponent(EntityHandle handle);
            
            template<class T, class = is_component_t<T>>
            static bool HasComponent(EntityHandle handle);

        private:
            static ecs::EcsImpl* m_impl;
    };

    template<> PointLight* Ecs::AddComponent<PointLight>(EntityHandle handle);
    template<> bool Ecs::RemoveComponent<PointLight>(EntityHandle handle);
    template<> PointLight* Ecs::GetComponent<PointLight>(EntityHandle handle);
    template<> bool Ecs::HasComponent<PointLight>(EntityHandle handle);

    template<> Renderer* Ecs::AddComponent<Renderer>(EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material);
    template<> bool Ecs::RemoveComponent<Renderer>(EntityHandle handle);
    template<> Renderer* Ecs::GetComponent<Renderer>(EntityHandle handle);
    template<> bool Ecs::HasComponent<Renderer>(EntityHandle handle);

    template<> Transform* Ecs::GetComponent<Transform>(EntityHandle handle);

    template<class T, class, class ... Args>
    T * Ecs::AddComponent(const EntityHandle handle, Args&& ... args)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->AddUserComponent<T>(std::forward<Args>(args)...);
    }

    template<class T, class>
    bool Ecs::RemoveComponent(const EntityHandle handle)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->RemoveUserComponent<T>();
    }

    template<class T, class>
    T * Ecs::GetComponent(const EntityHandle handle)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->GetUserComponent<T>();
    }

    template<class T, class>
    bool Ecs::HasComponent(const EntityHandle handle)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->HasUserComponent<T>();
    }
} // end namespace nc