#include "ECS.h"
#include "ecs/ECSImpl.h"
#include "component/Renderer.h"
#include "component/PointLight.h"

namespace nc
{
ecs::ECSImpl* ECS::m_impl = nullptr;

void ECS::RegisterImpl(ecs::ECSImpl* impl)
{
    m_impl = impl;
}

EntityHandle ECS::CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, std::string tag)
{
    auto impl = ECS::m_impl;
    auto entityHandle = impl->m_handleManager.GenerateNewHandle();
    auto transHandle = impl->m_transformSystem->Add(entityHandle, pos, rot, scale);
    impl->m_active.emplace(entityHandle, Entity{entityHandle, transHandle, std::move(tag)} );
    return entityHandle;
}

bool ECS::DestroyEntity(EntityHandle handle)
{
    auto impl = ECS::m_impl;
    if (!impl->DoesEntityExist(handle))
        return false;
    auto& containingMap = impl->GetMapContainingEntity(handle);
    impl->GetToDestroyEntities().emplace(handle, std::move(containingMap.at(handle)));
    containingMap.erase(handle);
    return true;
}

Entity * ECS::GetEntity(EntityHandle handle)
{
    auto impl = ECS::m_impl;
    if (!impl->DoesEntityExist(handle))
        return nullptr;

    auto& containingMap = impl->GetMapContainingEntity(handle);
    return &containingMap.at(handle); 
}

Entity * ECS::GetEntity(std::string tag)
{
    auto impl = ECS::m_impl;
    for(auto& pair : impl->m_active)
    {
        if(tag == pair.second.Tag)
        {
            return &pair.second;
        }
    }
    return nullptr;
}

template<> PointLight* ECS::AddComponent<PointLight>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");

    auto impl = ECS::m_impl;
    IF_THROW(impl->m_lightSystem->Contains(entity->Handles.pointLight), "Adding point light - entity already has a point light");

    auto lightHandle = impl->m_lightSystem->Add(handle);
    entity->Handles.pointLight = lightHandle;
    auto lightPtr = impl->m_lightSystem->GetPointerTo(lightHandle);
    lightPtr->Set({0.0f, 0.0f, 0.0f});
    return lightPtr;
}

template<> Renderer* ECS::AddComponent<Renderer>(EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");

    auto impl = ECS::m_impl;
    IF_THROW(impl->m_rendererSystem->Contains(entity->Handles.renderer), "Adding renderer - entity already has a renderer");

    auto rendererHandle = impl->m_rendererSystem->Add(handle, mesh, material);
    entity->Handles.renderer = rendererHandle;
    return impl->m_rendererSystem->GetPointerTo(rendererHandle);
}

template<> bool ECS::RemoveComponent<PointLight>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");
    return ECS::m_impl->m_lightSystem->Remove(entity->Handles.pointLight);
}

template<> bool ECS::RemoveComponent<Renderer>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");
    return ECS::m_impl->m_rendererSystem->Remove(entity->Handles.renderer);
}

template<> PointLight* ECS::GetComponent<PointLight>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");
    return ECS::m_impl->m_lightSystem->GetPointerTo(entity->Handles.pointLight);
}

template<> Renderer* ECS::GetComponent<Renderer>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");
    return ECS::m_impl->m_rendererSystem->GetPointerTo(entity->Handles.renderer);
}

template<> Transform* ECS::GetComponent<Transform>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");
    return ECS::m_impl->m_transformSystem->GetPointerTo(entity->Handles.transform);
}

    template<> bool ECS::HasComponent<PointLight>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");
    return ECS::m_impl->m_lightSystem->Contains(entity->Handles.pointLight);
}

template<> bool ECS::HasComponent<Renderer>(EntityHandle handle)
{
    auto entity = GetEntity(handle);
    IF_THROW(!entity, "Bad handle");
    return ECS::m_impl->m_rendererSystem->Contains(entity->Handles.renderer);
}
}