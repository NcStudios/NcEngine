#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/ColliderSystem.h"

namespace nc
{
    namespace internal
    {
        const auto DefaultEntityTag = std::string{"Entity"};
        ecs::EntityComponentSystem* g_ecsImpl = nullptr;
        ecs::EntitySystem* g_entitySystem = nullptr;
        ecs::ColliderSystem* g_colliderSystem = nullptr;
        ecs::ComponentSystem<NetworkDispatcher>* g_networkDispatcherSystem = nullptr;
        ecs::ComponentSystem<PointLight>* g_pointLightSystem = nullptr;
        ecs::ComponentSystem<Renderer>* g_rendererSystem = nullptr;
        ecs::ComponentSystem<Transform>* g_transformSystem = nullptr;
        
        void RegisterEcs(ecs::EntityComponentSystem* impl)
        {
            g_ecsImpl = impl;
            g_entitySystem = impl->GetEntitySystem();
            g_colliderSystem = impl->GetColliderSystem();
            g_networkDispatcherSystem = impl->GetNetworkDispatcherSystem();
            g_pointLightSystem = impl->GetPointLightSystem();
            g_rendererSystem = impl->GetRendererSystem();
            g_transformSystem = impl->GetTransformSystem();
        }
    }

    EntityHandle CreateEntity(EntityInfo info)
    {
        auto handle = internal::g_entitySystem->Add(info);
        internal::g_transformSystem->Add(handle, info.position, info.rotation, info.scale, info.parent);
        return handle;
    }

    void DestroyEntity(EntityHandle handle)
    {
        return internal::g_entitySystem->Remove(handle);
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
        IF_THROW(internal::g_ecsImpl->GetParticleEmitterSystem()->Contains(handle), "AddComponent<ParticleEmitter> - entity already has a particle emitter");
        return internal::g_ecsImpl->GetParticleEmitterSystem()->Add(handle, info);
    }

    template<> PointLight* AddComponent<PointLight>(EntityHandle handle)
    {
        return AddComponent<PointLight>(handle, PointLight::Properties{});
    }

    template<> PointLight* AddComponent<PointLight>(EntityHandle handle, PointLight::Properties properties)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<PointLight> - Bad handle");
        IF_THROW(internal::g_pointLightSystem->Contains(handle), "AddComponent<PointLight> - entity already has a point light");
        return internal::g_pointLightSystem->Add(handle, properties);
    }

    template<> Renderer* AddComponent<Renderer>(EntityHandle handle, graphics::Mesh mesh, graphics::Material material)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Renderer> - Bad handle");
        IF_THROW(internal::g_rendererSystem->Contains(handle), "AddComponent<Renderer> - entity already has a renderer");
        return internal::g_rendererSystem->Add(handle, std::move(mesh), std::move(material));
    }

    template<> NetworkDispatcher* AddComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<NetworkDispatcher> - Bad handle");
        IF_THROW(internal::g_networkDispatcherSystem->Contains(handle), "AddComponent<NetworkDispatcher> - entity already has a a NetworkDispatcher");
        return internal::g_networkDispatcherSystem->Add(handle);
    }

    template<> Collider* AddComponent<Collider>(EntityHandle handle, ColliderInfo info)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Collider> - Bad handle");
        IF_THROW(internal::g_colliderSystem->Contains(handle), "AddComponent<Collider> - entity already has a a Collider");
        return internal::g_colliderSystem->Add(handle, info);
    }

    template<> bool RemoveComponent<ParticleEmitter>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<ParticleEmitter> - Bad handle");
        return internal::g_ecsImpl->GetParticleEmitterSystem()->Remove(handle);
    }

    template<> bool RemoveComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<PointLight> - Bad handle");
        return internal::g_pointLightSystem->Remove(handle);
    }

    template<> bool RemoveComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Renderer> - Bad handle");
        return internal::g_rendererSystem->Remove(handle);
    }

    template<> bool RemoveComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<NetworkDispatcher> - Bad handle");
        return internal::g_networkDispatcherSystem->Remove(handle);
    }

    template<> bool RemoveComponent<Collider>(EntityHandle handle)
    {
        auto* entity = GetEntity(handle);
        IF_THROW(!entity, "RemoveComponent<Collider> - Bad handle");
        return internal::g_colliderSystem->Remove(handle, entity->IsStatic);
    }

    template<> ParticleEmitter* GetComponent<ParticleEmitter>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<ParticleEmitter> - Bad handle");
        return internal::g_ecsImpl->GetParticleEmitterSystem()->GetPointerTo(handle);
    }

    template<> PointLight* GetComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<PointLight> - Bad handle");
        return internal::g_pointLightSystem->GetPointerTo(handle);
    }

    template<> Renderer* GetComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Renderer> - Bad handle");
        return internal::g_rendererSystem->GetPointerTo(handle);
    }

    template<> Transform* GetComponent<Transform>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Transform> - Bad handle");
        return internal::g_transformSystem->GetPointerTo(handle);
    }

    template<> NetworkDispatcher* GetComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<NetworkDispatcher> - Bad handle");
        return internal::g_networkDispatcherSystem->GetPointerTo(handle);
    }

    template<> Collider* GetComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Collider> - Bad handle");
        return internal::g_colliderSystem->GetPointerTo(handle);
    }

    template<> bool HasComponent<ParticleEmitter>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<ParticleEmitter> - Bad handle");
        return internal::g_ecsImpl->GetParticleEmitterSystem()->Contains(handle);
    }

    template<> bool HasComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<PointLight> - Bad handle");
        return internal::g_pointLightSystem->Contains(handle);
    }

    template<> bool HasComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Renderer> - Bad handle");
        return internal::g_rendererSystem->Contains(handle);
    }

    template<> bool HasComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<NetworkDispatcher> - Bad handle");
        return internal::g_networkDispatcherSystem->Contains(handle);
    }

    template<> bool HasComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Collider> - Bad handle");
        return internal::g_colliderSystem->Contains(handle);
    }
}