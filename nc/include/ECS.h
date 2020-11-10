#pragma once

#include "ecs/Entity.h"
#include "component/PointLight.h"
#include "component/Renderer.h"
#include "component/Transform.h"
#include "NcDebug.h"

#include <string>

namespace nc
{
namespace ecs { class ECSImpl; }

class ECS
{
    template<class T>
    using is_component_t = typename std::enable_if_t<std::is_base_of_v<Component, T>>;

    public:
        static void RegisterImpl(ecs::ECSImpl* impl);

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
        static ecs::ECSImpl* m_impl;
};

template<> PointLight* ECS::AddComponent<PointLight>(EntityHandle handle);
template<> bool ECS::RemoveComponent<PointLight>(EntityHandle handle);
template<> PointLight* ECS::GetComponent<PointLight>(EntityHandle handle);
template<> bool ECS::HasComponent<PointLight>(EntityHandle handle);

template<> Renderer* ECS::AddComponent<Renderer>(EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material);
template<> bool ECS::RemoveComponent<Renderer>(EntityHandle handle);
template<> Renderer* ECS::GetComponent<Renderer>(EntityHandle handle);
template<> bool ECS::HasComponent<Renderer>(EntityHandle handle);

template<> Transform* ECS::GetComponent<Transform>(EntityHandle handle);

template<class T, class, class ... Args>
T * ECS::AddComponent(const EntityHandle handle, Args&& ... args)
{
    auto ptr = GetEntity(handle);
    IF_THROW(ptr == nullptr, "Bad handle");
    return ptr->AddUserComponent<T>(std::forward<Args>(args)...);
}

template<class T, class>
bool ECS::RemoveComponent(const EntityHandle handle)
{
    auto ptr = GetEntity(handle);
    IF_THROW(ptr == nullptr, "Bad handle");
    return ptr->RemoveUserComponent<T>();
}

template<class T, class>
T * ECS::GetComponent(const EntityHandle handle)
{
    auto ptr = GetEntity(handle);
    IF_THROW(ptr == nullptr, "Bad handle");
    return ptr->GetUserComponent<T>();
}

template<class T, class>
bool ECS::HasComponent(const EntityHandle handle)
{
    auto ptr = GetEntity(handle);
    IF_THROW(ptr == nullptr, "Bad handle");
    return ptr->HasUserComponent<T>();
}
} // end namespace nc