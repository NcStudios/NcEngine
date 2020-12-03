#include "Ecs.h"
#include "Ecs/EcsImpl.h"

namespace nc
{
    ecs::EcsImpl* Ecs::m_impl = nullptr;

    void Ecs::RegisterImpl(ecs::EcsImpl* impl)
    {
        m_impl = impl;
    }

    EntityHandle Ecs::CreateEntity(std::string tag)
    {
        return Ecs::CreateEntity(Vector3::Zero(), Vector3::Zero(), Vector3::One(), std::move(tag));
    }

    EntityHandle Ecs::CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, std::string tag)
    {
        auto impl = Ecs::m_impl;
        auto entityHandle = impl->m_handleManager.GenerateNewHandle();
        auto transHandle = impl->m_transformSystem->Add(entityHandle, pos, rot, scale);
        impl->m_active.emplace(entityHandle, Entity{entityHandle, transHandle, std::move(tag)} );
        return entityHandle;
    }

    bool Ecs::DestroyEntity(EntityHandle handle)
    {
        auto impl = Ecs::m_impl;
        if (!impl->DoesEntityExist(handle))
            return false;
        auto& containingMap = impl->GetMapContainingEntity(handle);
        impl->GetToDestroyEntities().emplace(handle, std::move(containingMap.at(handle)));
        containingMap.erase(handle);
        return true;
    }

    Entity * Ecs::GetEntity(EntityHandle handle)
    {
        auto impl = Ecs::m_impl;
        if (!impl->DoesEntityExist(handle))
            return nullptr;

        auto& containingMap = impl->GetMapContainingEntity(handle);
        return &containingMap.at(handle); 
    }

    Entity * Ecs::GetEntity(std::string tag)
    {
        auto impl = Ecs::m_impl;
        for(auto& pair : impl->m_active)
        {
            if(tag == pair.second.Tag)
            {
                return &pair.second;
            }
        }
        return nullptr;
    }

    template<> PointLight* Ecs::AddComponent<PointLight>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");

        auto impl = Ecs::m_impl;
        IF_THROW(impl->m_lightSystem->Contains(entity->Handles.pointLight), "Adding point light - entity already has a point light");

        auto lightHandle = impl->m_lightSystem->Add(handle);
        entity->Handles.pointLight = lightHandle;
        auto lightPtr = impl->m_lightSystem->GetPointerTo(lightHandle);
        lightPtr->Set({0.0f, 0.0f, 0.0f});
        return lightPtr;
    }

    template<> Renderer* Ecs::AddComponent<Renderer>(EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");

        auto impl = Ecs::m_impl;
        IF_THROW(impl->m_rendererSystem->Contains(entity->Handles.renderer), "Adding renderer - entity already has a renderer");

        auto rendererHandle = impl->m_rendererSystem->Add(handle, mesh, material);
        entity->Handles.renderer = rendererHandle;
        return impl->m_rendererSystem->GetPointerTo(rendererHandle);
    }

    template<> NetworkDispatcher* Ecs::AddComponent<NetworkDispatcher>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");

        auto impl = Ecs::m_impl;
        IF_THROW(impl->m_networkDispatcherSystem->Contains(entity->Handles.networkDispatcher), "Adding NetworkDispatcher - entity already has a a NetworkDispatcher");

        auto dispatcherHandle = impl->m_networkDispatcherSystem->Add(handle);
        entity->Handles.networkDispatcher = dispatcherHandle;
        return impl->m_networkDispatcherSystem->GetPointerTo(dispatcherHandle);
    }

    template<> bool Ecs::RemoveComponent<PointLight>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_lightSystem->Remove(entity->Handles.pointLight);
    }

    template<> bool Ecs::RemoveComponent<Renderer>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_rendererSystem->Remove(entity->Handles.renderer);
    }

    template<> bool Ecs::RemoveComponent<NetworkDispatcher>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_networkDispatcherSystem->Remove(entity->Handles.networkDispatcher);
    }

    template<> PointLight* Ecs::GetComponent<PointLight>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_lightSystem->GetPointerTo(entity->Handles.pointLight);
    }

    template<> Renderer* Ecs::GetComponent<Renderer>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_rendererSystem->GetPointerTo(entity->Handles.renderer);
    }

    template<> Transform* Ecs::GetComponent<Transform>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_transformSystem->GetPointerTo(entity->Handles.transform);
    }

    template<> NetworkDispatcher* Ecs::GetComponent<NetworkDispatcher>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_networkDispatcherSystem->GetPointerTo(entity->Handles.networkDispatcher);
    }

    template<> bool Ecs::HasComponent<PointLight>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_lightSystem->Contains(entity->Handles.pointLight);
    }

    template<> bool Ecs::HasComponent<Renderer>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_rendererSystem->Contains(entity->Handles.renderer);
    }

    template<> bool Ecs::HasComponent<NetworkDispatcher>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return Ecs::m_impl->m_networkDispatcherSystem->Contains(entity->Handles.networkDispatcher);
    }
}