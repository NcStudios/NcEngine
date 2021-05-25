#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"

namespace nc
{
    namespace internal
    {
        const auto DefaultEntityTag = std::string{"Entity"};
        ecs::EntityComponentSystem* g_ecsImpl = nullptr;
        ecs::EntitySystem* g_entitySystem = nullptr;
        ecs::registry_type* g_registry = nullptr;
        ecs::ParticleEmitterSystem* g_particleEmitterSystem = nullptr;

        void RegisterEcs(ecs::EntityComponentSystem* impl)
        {
            g_ecsImpl = impl;
            g_entitySystem = impl->GetEntitySystem();
            g_registry = impl->GetRegistry();
            g_particleEmitterSystem = impl->GetParticleEmitterSystem();
        }
    }

    EntityHandle CreateEntity(EntityInfo info)
    {
        auto handle = internal::g_entitySystem->Add(info);
        internal::g_registry->AddComponent<Transform>(handle, info.position, info.rotation, info.scale, info.parent);
        return handle;
    }

    void DestroyEntity(EntityHandle handle)
    {
        internal::g_entitySystem->Remove(handle);
    }

    bool EntityExists(EntityHandle handle)
    {
        return internal::g_entitySystem->Contains(handle);
    }

    Entity* GetEntity(EntityHandle handle)
    {
        return internal::g_entitySystem->Get(handle);
    }

    Entity* GetEntity(const std::string& tag)
    {
        return internal::g_entitySystem->Get(tag);
    }

    template<> ParticleEmitter* AddComponent<ParticleEmitter>(EntityHandle handle, ParticleInfo info)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<ParticleEmitter> - Bad handle");
        return internal::g_registry->AddComponent<ParticleEmitter>(handle, info, internal::g_particleEmitterSystem);
    }

    template<> PointLight* AddComponent<PointLight>(EntityHandle handle)
    {
        return internal::g_registry->AddComponent<PointLight>(handle, PointLight::Properties{});
    }

    template<> PointLight* AddComponent<PointLight>(EntityHandle handle, PointLight::Properties properties)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<PointLight> - Bad handle");
        return internal::g_registry->AddComponent<PointLight>(handle, properties);
    }

    template<> Renderer* AddComponent<Renderer>(EntityHandle handle, graphics::Mesh mesh, graphics::Material material)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Renderer> - Bad handle");
        return internal::g_registry->AddComponent<Renderer>(handle, std::move(mesh), std::move(material));
    }

    template<> NetworkDispatcher* AddComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<NetworkDispatcher> - Bad handle");
        return internal::g_registry->AddComponent<NetworkDispatcher>(handle);
    }

    template<> Collider* AddComponent<Collider>(EntityHandle handle, ColliderInfo info)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Collider> - Bad handle");
        return internal::g_registry->AddComponent<Collider>(handle, info);
    }

    template<> bool RemoveComponent<ParticleEmitter>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<ParticleEmitter> - Bad handle");
        return internal::g_registry->RemoveComponent<ParticleEmitter>(handle);
    }

    template<> bool RemoveComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<PointLight> - Bad handle");
        return internal::g_registry->RemoveComponent<PointLight>(handle);
    }

    template<> bool RemoveComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Renderer> - Bad handle");
        return internal::g_registry->RemoveComponent<Renderer>(handle);
    }

    template<> bool RemoveComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<NetworkDispatcher> - Bad handle");
        return internal::g_registry->RemoveComponent<NetworkDispatcher>(handle);
    }

    template<> bool RemoveComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Collider> - Bad handle");
        return internal::g_registry->RemoveComponent<Collider>(handle);
    }

    template<> ParticleEmitter* GetComponent<ParticleEmitter>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<ParticleEmitter> - Bad handle");
        return internal::g_registry->GetComponent<ParticleEmitter>(handle);
    }

    template<> PointLight* GetComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<PointLight> - Bad handle");
        return internal::g_registry->GetComponent<PointLight>(handle);
    }

    template<> Renderer* GetComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Renderer> - Bad handle");
        return internal::g_registry->GetComponent<Renderer>(handle);
    }

    template<> Transform* GetComponent<Transform>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Transform> - Bad handle");
        return internal::g_registry->GetComponent<Transform>(handle);
    }

    template<> NetworkDispatcher* GetComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<NetworkDispatcher> - Bad handle");
        return internal::g_registry->GetComponent<NetworkDispatcher>(handle);
    }

    template<> Collider* GetComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Collider> - Bad handle");
        return internal::g_registry->GetComponent<Collider>(handle);
    }

    template<> bool HasComponent<ParticleEmitter>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<ParticleEmitter> - Bad handle");
        return internal::g_registry->HasComponent<ParticleEmitter>(handle);
    }

    template<> bool HasComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<PointLight> - Bad handle");
        return internal::g_registry->HasComponent<PointLight>(handle);
    }

    template<> bool HasComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Renderer> - Bad handle");
        return internal::g_registry->HasComponent<Renderer>(handle);
    }

    template<> bool HasComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<NetworkDispatcher> - Bad handle");
        return internal::g_registry->HasComponent<NetworkDispatcher>(handle);
    }

    template<> bool HasComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Collider> - Bad handle");
        return internal::g_registry->HasComponent<Collider>(handle);
    }
}