#include "Ecs.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/ColliderSystem.h"
#ifdef USE_VULKAN
    #include "ecs/RendererSystem.h"
#endif

namespace nc
{
    namespace internal
    {
        const auto DefaultEntityTag = std::string{"Entity"};
        ecs::EntityComponentSystem* g_ecsImpl = nullptr;
        ecs::ColliderSystem* g_colliderSystem = nullptr;
        ecs::ComponentSystem<NetworkDispatcher>* g_networkDispatcherSystem = nullptr;
        ecs::ComponentSystem<PointLight>* g_pointLightSystem = nullptr;
        ecs::ComponentSystem<Renderer>* g_rendererSystem = nullptr;
        ecs::ComponentSystem<Transform>* g_transformSystem = nullptr;

        void RegisterEcs(ecs::EntityComponentSystem* impl)
        {
            g_ecsImpl = impl;
            g_colliderSystem = impl->GetColliderSystem();
            g_networkDispatcherSystem = impl->GetNetworkDispatcherSystem();
            g_pointLightSystem = impl->GetPointLightSystem();
            g_rendererSystem = impl->GetRendererSystem();
            g_transformSystem = impl->GetTransformSystem();
        }

        #ifdef USE_VULKAN
        ecs::RendererSystem* g_rendererSystemImpl = nullptr;
        void RegisterRendererSystem(ecs::RendererSystem* impl)
        {
            g_rendererSystemImpl = impl;
        }
        #endif
    }

    EntityHandle CreateEntity(EntityInfo info)
    {
        return internal::g_ecsImpl->CreateEntity(std::move(info));
    }

    bool DestroyEntity(EntityHandle handle)
    {
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
        IF_THROW(internal::g_pointLightSystem->Contains(handle), "AddComponent<PointLight> - entity already has a point light");
        auto lightPtr = internal::g_pointLightSystem->Add(handle, properties);
        return lightPtr;
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

    #ifdef USE_VULKAN
    template<> vulkan::Renderer* AddComponent<vulkan::Renderer>(EntityHandle handle, std::string meshUid, graphics::vulkan::TechniqueType techniqueType)
    {
        IF_THROW(!GetEntity(handle), "AddComponent<Renderer> - Bad handle");
        IF_THROW(internal::g_rendererSystemImpl->Contains(handle), "AddComponent<Renderer> - entity already has a renderer");
        return internal::g_rendererSystemImpl->Add(handle, std::move(meshUid), techniqueType);
    }

    template<> bool RemoveComponent<vulkan::Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "RemoveComponent<Renderer> - Bad handle");
        return internal::g_rendererSystemImpl->Remove(handle);
    }

    template<> bool HasComponent<vulkan::Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "HasComponent<Renderer> - Bad handle");
        return internal::g_rendererSystemImpl->Contains(handle);
    }

    template<> vulkan::Renderer* GetComponent<vulkan::Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "GetComponent<Renderer> - Bad handle");
        return internal::g_rendererSystemImpl->GetPointerTo(handle);
    }
    #endif
}