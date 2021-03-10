#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"
#include "physics/ColliderSystem.h"

namespace nc
{
    namespace internal
    {
        const auto DefaultEntityTag = std::string{"Entity"};
        ecs::EntityComponentSystem* g_ecsImpl = nullptr;
        physics::ColliderSystem* g_colliderSystemImpl = nullptr;

        void RegisterEcs(ecs::EntityComponentSystem* impl)
        {
            g_ecsImpl = impl;
        }

        void RegisterColliderSystem(physics::ColliderSystem* impl)
        {
            g_colliderSystemImpl = impl;
        }
    }

    EntityHandle CreateEntity(EntityInfo info)
    {
        return internal::g_ecsImpl->CreateEntity(std::move(info));
    }

    bool DestroyEntity(EntityHandle handle)
    {
        internal::g_colliderSystemImpl->Remove(handle);
        return internal::g_ecsImpl->DestroyEntity(handle);
    }

    Entity* GetEntity(EntityHandle handle)
    {
        return internal::g_ecsImpl->GetEntity(handle);
    }

    Entity* GetEntity(const std::string& tag)
    {
        return internal::g_ecsImpl->GetEntity(tag);
    }

    template<> PointLight* AddComponent<PointLight>(EntityHandle handle)
    {
        return AddComponent<PointLight>(handle, PointLight::Properties{});
    }

    template<> PointLight* AddComponent<PointLight>(EntityHandle handle, PointLight::Properties properties)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<PointLight> - Bad handle");
        IF_THROW(internal::g_ecsImpl->GetSystem<PointLight>()->Contains(handle), "AddComponent<PointLight> - entity already has a point light");
        auto lightPtr = internal::g_ecsImpl->GetSystem<PointLight>()->Add(handle, properties);
        return lightPtr;
    }

    template<> Renderer* AddComponent<Renderer>(EntityHandle handle, graphics::Mesh mesh, graphics::Material material)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Renderer> - Bad handle");
        IF_THROW(internal::g_ecsImpl->GetSystem<Renderer>()->Contains(handle), "AddComponent<Renderer> - entity already has a renderer");
        return internal::g_ecsImpl->GetSystem<Renderer>()->Add(handle, std::move(mesh), std::move(material));
    }

    template<> NetworkDispatcher* AddComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<NetworkDispatcher> - Bad handle");
        IF_THROW(internal::g_ecsImpl->GetSystem<NetworkDispatcher>()->Contains(handle), "AddComponent<NetworkDispatcher> - entity already has a a NetworkDispatcher");
        return internal::g_ecsImpl->GetSystem<NetworkDispatcher>()->Add(handle);
    }

    template<> Collider* AddComponent<Collider>(EntityHandle handle, ColliderInfo info)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Collider> - Bad handle");
        IF_THROW(internal::g_colliderSystemImpl->Contains(handle), "AddComponent<Collider> - entity already has a a Collider");
        return internal::g_colliderSystemImpl->Add(handle, info);
    }

    template<> bool RemoveComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<PointLight> - Bad handle");
        return internal::g_ecsImpl->GetSystem<PointLight>()->Remove(handle);
    }

    template<> bool RemoveComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Renderer> - Bad handle");
        return internal::g_ecsImpl->GetSystem<Renderer>()->Remove(handle);
    }

    template<> bool RemoveComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<NetworkDispatcher> - Bad handle");
        return internal::g_ecsImpl->GetSystem<NetworkDispatcher>()->Remove(handle);
    }

    template<> bool RemoveComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Collider> - Bad handle");
        return internal::g_colliderSystemImpl->Remove(handle);
    }

    template<> PointLight* GetComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<PointLight> - Bad handle");
        return internal::g_ecsImpl->GetSystem<PointLight>()->GetPointerTo(handle);
    }

    template<> Renderer* GetComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Renderer> - Bad handle");
        return internal::g_ecsImpl->GetSystem<Renderer>()->GetPointerTo(handle);
    }

    template<> Transform* GetComponent<Transform>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Transform> - Bad handle");
        return internal::g_ecsImpl->GetSystem<Transform>()->GetPointerTo(handle);
    }

    template<> NetworkDispatcher* GetComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<NetworkDispatcher> - Bad handle");
        return internal::g_ecsImpl->GetSystem<NetworkDispatcher>()->GetPointerTo(handle);
    }

    template<> Collider* GetComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Collider> - Bad handle");
        return internal::g_colliderSystemImpl->GetPointerTo(handle);
    }

    template<> bool HasComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<PointLight> - Bad handle");
        return internal::g_ecsImpl->GetSystem<PointLight>()->Contains(handle);
    }

    template<> bool HasComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Renderer> - Bad handle");
        return internal::g_ecsImpl->GetSystem<Renderer>()->Contains(handle);
    }

    template<> bool HasComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<NetworkDispatcher> - Bad handle");
        return internal::g_ecsImpl->GetSystem<NetworkDispatcher>()->Contains(handle);
    }

    template<> bool HasComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Collider> - Bad handle");
        return internal::g_colliderSystemImpl->Contains(handle);
    }
}