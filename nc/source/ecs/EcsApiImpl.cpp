#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"

namespace nc
{
    namespace internal
    {
        const auto DefaultEntityTag = std::string{"Entity"};
        ecs::EntityComponentSystem* g_impl = nullptr;

        void RegisterEcs(ecs::EntityComponentSystem* impl)
        {
            g_impl = impl;
        }
    }

    EntityHandle CreateEntity()
    {
        return CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::One(), internal::DefaultEntityTag);
    }

    EntityHandle CreateEntity(std::string tag)
    {
        return CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::One(), std::move(tag));
    }

    EntityHandle CreateEntity(Vector3 pos, Quaternion rot, Vector3 scale, std::string tag)
    {
        IF_THROW(scale == Vector3::Zero(), "CreateEntity - scale cannot be Vector3::Zero");
        return internal::g_impl->CreateEntity(pos, rot, scale, std::move(tag));
    }

    bool DestroyEntity(EntityHandle handle)
    {
        return internal::g_impl->DestroyEntity(handle);
    }

    Entity* GetEntity(EntityHandle handle)
    {
        return internal::g_impl->GetEntity(handle);
    }

    Entity* GetEntity(const std::string& tag)
    {
        return internal::g_impl->GetEntity(tag);
    }

    template<> PointLight* AddComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<PointLight> - Bad handle");
        IF_THROW(internal::g_impl->GetSystem<PointLight>()->Contains(handle), "AddComponent<PointLight> - entity already has a point light");

        auto lightPtr = internal::g_impl->GetSystem<PointLight>()->Add(handle);
        return lightPtr;
    }

    template<> Renderer* AddComponent<Renderer>(EntityHandle handle, graphics::Mesh mesh, graphics::Material material)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Renderer> - Bad handle");
        IF_THROW(internal::g_impl->GetSystem<Renderer>()->Contains(handle), "AddComponent<Renderer> - entity already has a renderer");

        return internal::g_impl->GetSystem<Renderer>()->Add(handle, std::move(mesh), std::move(material));
    }

    template<> NetworkDispatcher* AddComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<NetworkDispatcher> - Bad handle");
        IF_THROW(internal::g_impl->GetSystem<NetworkDispatcher>()->Contains(handle), "AddComponent<NetworkDispatcher> - entity already has a a NetworkDispatcher");

        return internal::g_impl->GetSystem<NetworkDispatcher>()->Add(handle);
    }

    template<> Collider* AddComponent<Collider>(EntityHandle handle, Vector3 scale)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Collider> - Bad handle");
        IF_THROW(internal::g_impl->GetSystem<Collider>()->Contains(handle), "AddComponent<Collider> - entity already has a a Collider");

        return internal::g_impl->GetSystem<Collider>()->Add(handle, scale);
    }

    template<> bool RemoveComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<PointLight> - Bad handle");
        return internal::g_impl->GetSystem<PointLight>()->Remove(handle);
    }

    template<> bool RemoveComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Renderer> - Bad handle");
        return internal::g_impl->GetSystem<Renderer>()->Remove(handle);
    }

    template<> bool RemoveComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<NetworkDispatcher> - Bad handle");
        return internal::g_impl->GetSystem<NetworkDispatcher>()->Remove(handle);
    }

    template<> bool RemoveComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Collider> - Bad handle");
        return internal::g_impl->GetSystem<Collider>()->Remove(handle);
    }

    template<> PointLight* GetComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<PointLight> - Bad handle");
        return internal::g_impl->GetSystem<PointLight>()->GetPointerTo(handle);
    }

    template<> Renderer* GetComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Renderer> - Bad handle");
        return internal::g_impl->GetSystem<Renderer>()->GetPointerTo(handle);
    }

    template<> Transform* GetComponent<Transform>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Transform> - Bad handle");
        return internal::g_impl->GetSystem<Transform>()->GetPointerTo(handle);
    }

    template<> NetworkDispatcher* GetComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<NetworkDispatcher> - Bad handle");
        return internal::g_impl->GetSystem<NetworkDispatcher>()->GetPointerTo(handle);
    }

    template<> Collider* GetComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Collider> - Bad handle");
        return internal::g_impl->GetSystem<Collider>()->GetPointerTo(handle);
    }

    template<> bool HasComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<PointLight> - Bad handle");
        return internal::g_impl->GetSystem<PointLight>()->Contains(handle);
    }

    template<> bool HasComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Renderer> - Bad handle");
        return internal::g_impl->GetSystem<Renderer>()->Contains(handle);
    }

    template<> bool HasComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<NetworkDispatcher> - Bad handle");
        return internal::g_impl->GetSystem<NetworkDispatcher>()->Contains(handle);
    }

    template<> bool HasComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Collider> - Bad handle");
        return internal::g_impl->GetSystem<Collider>()->Contains(handle);
    }
}