#pragma once

#include "entity/Entity.h"
#include "entity/EntityInfo.h"
#include "component/Collider.h"
#include "component/PointLight.h"
#include "component/NetworkDispatcher.h"
#include "component/Renderer.h"
#include "component/Transform.h"
#include "debug/Utils.h"

#include <concepts>
#include <string>

namespace nc
{
    /** Entity Functions */
    EntityHandle CreateEntity(EntityInfo info = EntityInfo{});
    bool DestroyEntity(EntityHandle handle);
    [[nodiscard]] Entity* GetEntity(EntityHandle handle);
    [[nodiscard]] Entity* GetEntity(const std::string& tag);

    /** Component Functions */
    template<std::derived_from<ComponentBase> T, class ...Args>
    T* AddComponent(EntityHandle handle, Args ... args);
    
    template<std::derived_from<ComponentBase> T>
    bool RemoveComponent(EntityHandle handle);
    
    template<std::derived_from<ComponentBase> T>
    [[nodiscard]] T* GetComponent(EntityHandle handle);
    
    template<std::derived_from<ComponentBase> T>
    [[nodiscard]] bool HasComponent(EntityHandle handle);

    /** Specializations for engine components */
    template<> PointLight* AddComponent<PointLight>(EntityHandle handle, PointLight::Properties properties);
    template<> PointLight* AddComponent<PointLight>(EntityHandle handle);
    template<> PointLight* GetComponent<PointLight>(EntityHandle handle);
    template<> bool HasComponent<PointLight>(EntityHandle handle);
    template<> bool RemoveComponent<PointLight>(EntityHandle handle);

    template<> Renderer* AddComponent<Renderer>(EntityHandle handle, graphics::Mesh mesh, graphics::Material material);
    template<> Renderer* GetComponent<Renderer>(EntityHandle handle);
    template<> bool HasComponent<Renderer>(EntityHandle handle);
    template<> bool RemoveComponent<Renderer>(EntityHandle handle);

    template<> NetworkDispatcher* AddComponent<NetworkDispatcher>(EntityHandle handle);
    template<> NetworkDispatcher* GetComponent<NetworkDispatcher>(EntityHandle handle);
    template<> bool HasComponent<NetworkDispatcher>(EntityHandle handle);
    template<> bool RemoveComponent<NetworkDispatcher>(EntityHandle handle);

    template<> Transform* GetComponent<Transform>(EntityHandle handle);

    template<> Collider* AddComponent<Collider>(EntityHandle handle, ColliderType type, Vector3 scale);
    template<> Collider* GetComponent<Collider>(EntityHandle handle);
    template<> bool HasComponent<Collider>(EntityHandle handle);
    template<> bool RemoveComponent<Collider>(EntityHandle handle);

    /** Template definitions for custom componenets */
    template<std::derived_from<ComponentBase> T, class ... Args>
    T * AddComponent(const EntityHandle handle, Args ... args)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->AddUserComponent<T>(std::forward<Args>(args)...);
    }

    template<std::derived_from<ComponentBase> T>
    bool RemoveComponent(const EntityHandle handle)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->RemoveUserComponent<T>();
    }

    template<std::derived_from<ComponentBase> T>
    T * GetComponent(const EntityHandle handle)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->GetUserComponent<T>();
    }

    template<std::derived_from<ComponentBase> T>
    bool HasComponent(const EntityHandle handle)
    {
        auto ptr = GetEntity(handle);
        IF_THROW(ptr == nullptr, "Bad handle");
        return ptr->HasUserComponent<T>();
    }

    /** Internal use */
    namespace ecs { class EntityComponentSystem; }
    namespace internal
    {
        void RegisterEcs(ecs::EntityComponentSystem* impl);
    }
} // end namespace nc