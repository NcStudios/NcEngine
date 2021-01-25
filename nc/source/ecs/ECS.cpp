#include "Ecs.h"
#include "Ecs/EcsImpl.h"

namespace
{
    const auto DefaultEntityTag = "Entity";
}

namespace nc
{
    ecs::EcsImpl* Ecs::m_impl = nullptr;

    void Ecs::RegisterImpl(ecs::EcsImpl* impl)
    {
        m_impl = impl;
    }

    EntityHandle Ecs::CreateEntity()
    {
        return Ecs::CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::One(), DefaultEntityTag);
    }

    EntityHandle Ecs::CreateEntity(std::string tag)
    {
        return Ecs::CreateEntity(Vector3::Zero(), Quaternion::Identity(), Vector3::One(), std::move(tag));
    }

    EntityHandle Ecs::CreateEntity(Vector3 pos, Quaternion rot, Vector3 scale, std::string tag)
    {
        IF_THROW(scale == Vector3::Zero(), "Ecs::CreateEntity - scale cannot be Vector3::Zero");
        auto impl = Ecs::m_impl;
        auto entityHandle = impl->m_handleManager.GenerateNewHandle();
        impl->m_transformSystem->Add(entityHandle, pos, rot, scale);
        impl->m_active.emplace(entityHandle, Entity{entityHandle, std::move(tag)} );
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
        IF_THROW(!GetEntity(handle), "Bad handle");
        IF_THROW(Ecs::m_impl->m_lightSystem->Contains(handle), "Adding point light - entity already has a point light");

        auto lightPtr = Ecs::m_impl->m_lightSystem->Add(handle);
        lightPtr->Set({0.0f, 0.0f, 0.0f});
        return lightPtr;
    }

    template<> Renderer* Ecs::AddComponent<Renderer>(EntityHandle handle, graphics::Mesh mesh, graphics::Material material)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        IF_THROW(Ecs::m_impl->m_rendererSystem->Contains(handle), "Adding renderer - entity already has a renderer");

        return Ecs::m_impl->m_rendererSystem->Add(handle, std::move(mesh), std::move(material));
    }

    template<> NetworkDispatcher* Ecs::AddComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        IF_THROW(Ecs::m_impl->m_networkDispatcherSystem->Contains(handle), "Adding NetworkDispatcher - entity already has a a NetworkDispatcher");

        return Ecs::m_impl->m_networkDispatcherSystem->Add(handle);
    }

    template<> Collider* Ecs::AddComponent<Collider>(EntityHandle handle, Vector3 scale)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        IF_THROW(Ecs::m_impl->m_colliderSystem->Contains(handle), "Adding Collider - entity already has a a Collider");

        return Ecs::m_impl->m_colliderSystem->Add(handle, scale);
    }

    template<> bool Ecs::RemoveComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_lightSystem->Remove(handle);
    }

    template<> bool Ecs::RemoveComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_rendererSystem->Remove(handle);
    }

    template<> bool Ecs::RemoveComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_networkDispatcherSystem->Remove(handle);
    }

    template<> bool Ecs::RemoveComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_colliderSystem->Remove(handle);
    }

    template<> PointLight* Ecs::GetComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_lightSystem->GetPointerTo(handle);
    }

    template<> Renderer* Ecs::GetComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_rendererSystem->GetPointerTo(handle);
    }

    template<> Transform* Ecs::GetComponent<Transform>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_transformSystem->GetPointerTo(handle);
    }

    template<> NetworkDispatcher* Ecs::GetComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_networkDispatcherSystem->GetPointerTo(handle);
    }

    template<> Collider* Ecs::GetComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_colliderSystem->GetPointerTo(handle);
    }

    template<> bool Ecs::HasComponent<PointLight>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_lightSystem->Contains(handle);
    }

    template<> bool Ecs::HasComponent<Renderer>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_rendererSystem->Contains(handle);
    }

    template<> bool Ecs::HasComponent<NetworkDispatcher>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_networkDispatcherSystem->Contains(handle);
    }

    template<> bool Ecs::HasComponent<Collider>(EntityHandle handle)
    {
        IF_THROW(!GetEntity(handle), "Bad handle");
        return Ecs::m_impl->m_colliderSystem->Contains(handle);
    }
}