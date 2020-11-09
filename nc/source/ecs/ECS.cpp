#include "ECS.h"
#include "LightSystem.h"
#include "RenderingSystem.h"
#include "TransformSystem.h"
#include "component/Renderer.h"
#include "component/PointLight.h"

namespace nc::ecs
{
    ECS* ECS::m_instance = nullptr;

    ECS::ECS(LightSystem* light, RenderingSystem* rend, TransformSystem* trans)
        : m_active{},
          m_toDestroy{},
          m_lightSystem{ light },
          m_renderingSystem{ rend },
          m_transformSystem{ trans }
    {
        ECS::m_instance = this;
    }

    ECS::~ECS()
    {
        ECS::m_instance = nullptr;
    }

    RenderingSystem* ECS::GetRenderingSystem()
    {
        return ECS::m_instance->m_renderingSystem;
    }

    EntityHandle ECS::CreateEntity(Vector3 pos, Vector3 rot, Vector3 scale, std::string tag)
    {
        auto es = ECS::m_instance;
        auto entityHandle = es->handleManager.GenerateNewHandle();
        auto transHandle = es->m_transformSystem->Add(entityHandle, pos, rot, scale);
        es->m_active.emplace(entityHandle, Entity{entityHandle, transHandle, std::move(tag)} );
        return entityHandle;
    }

    bool ECS::DestroyEntity(EntityHandle handle)
    {
        auto es = ECS::m_instance;
        if (!es->DoesEntityExist(handle))
            return false;
        auto& containingMap = es->GetMapContainingEntity(handle);
        es->GetToDestroyEntities().emplace(handle, std::move(containingMap.at(handle)));
        containingMap.erase(handle);
        return true;
    }

    Entity * ECS::GetEntity(EntityHandle handle)
    {
        auto es = ECS::m_instance;
        if (!es->DoesEntityExist(handle))
            return nullptr;

        auto& containingMap = es->GetMapContainingEntity(handle);
        return &containingMap.at(handle); 
    }

    Entity * ECS::GetEntity(std::string tag)
    {
        auto es = ECS::m_instance;
        for(auto& pair : es->m_active)
        {
            if(tag == pair.second.Tag)
            {
                return &pair.second;
            }
        }
        return nullptr;
    }

    Transform* ECS::GetTransformFromComponentHandle(ComponentHandle transformHandle)
    {
        auto ts = ECS::m_instance->m_transformSystem;
        auto exists = ts->Contains(transformHandle);
        IF_THROW(!exists, "Bad handle");
        return ts->GetPointerTo(transformHandle);
    }
    
    Transform* ECS::GetTransformFromEntityHandle(EntityHandle entityHandle)
    {
        auto entity = GetEntity(entityHandle);
        IF_THROW(!entity, "Bad handle");
        return ECS::m_instance->m_transformSystem->GetPointerTo(entity->Handles.transform);
    }

    template<> bool ECS::HasComponent<PointLight>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return ECS::m_instance->m_lightSystem->Contains(entity->Handles.pointLight);
    }

    template<> bool ECS::HasComponent<Renderer>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return ECS::m_instance->m_renderingSystem->Contains(entity->Handles.renderer);
    }

    template<> bool ECS::RemoveComponent<PointLight>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return ECS::m_instance->m_lightSystem->Remove(entity->Handles.pointLight);
    }

    template<> bool ECS::RemoveComponent<Renderer>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return ECS::m_instance->m_renderingSystem->Remove(entity->Handles.renderer);
    }

    template<> PointLight* ECS::GetComponent<PointLight>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return ECS::m_instance->m_lightSystem->GetPointerTo(entity->Handles.pointLight);
    }

    template<> Renderer* ECS::GetComponent<Renderer>(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        return ECS::m_instance->m_renderingSystem->GetPointerTo(entity->Handles.renderer);
    }

    PointLight* ECS::AddPointLight(EntityHandle handle)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");
        IF_THROW(ECS::m_instance->m_lightSystem->Contains(entity->Handles.pointLight), "Adding point light - entity already has a point light");

        auto lightHandle = ECS::m_instance->m_lightSystem->Add(handle);
        entity->Handles.pointLight = lightHandle;
        auto lightPtr = ECS::m_instance->m_lightSystem->GetPointerTo(lightHandle);
        lightPtr->Set({0.0f, 0.0f, 0.0f});
        return lightPtr;
    }

    Renderer* ECS::AddRenderer(EntityHandle handle, graphics::Mesh& mesh, graphics::PBRMaterial& material)
    {
        auto entity = GetEntity(handle);
        IF_THROW(!entity, "Bad handle");

        IF_THROW(ECS::m_instance->m_renderingSystem->Contains(entity->Handles.renderer), "Adding renderer - entity already has a renderer");

        auto rendererHandle = ECS::m_instance->m_renderingSystem->Add(handle, mesh, material);
        entity->Handles.renderer = rendererHandle;
        return ECS::m_instance->m_renderingSystem->GetPointerTo(rendererHandle);
    }

    bool ECS::DoesEntityExist(const EntityHandle handle) noexcept
    {
        return m_active.count(handle) > 0;
    }

    std::unordered_map<EntityHandle, Entity> & ECS::GetMapContainingEntity(const EntityHandle handle, bool checkAll) noexcept(false)
    {
        if (m_active.count(handle) > 0)
            return m_active;

        if (checkAll && (m_toDestroy.count(handle) > 0) ) //only check toDestroy if checkAll flag is set
            return m_toDestroy;

        throw std::runtime_error("Engine::GetmapContainingEntity() - Entity not found.");
    }

    std::unordered_map<EntityHandle, Entity> & ECS::GetActiveEntities() noexcept
    {
        return m_active;
    }

    std::unordered_map<EntityHandle, Entity> & ECS::GetToDestroyEntities() noexcept
    {
        return m_toDestroy;
    }

    Entity * ECS::GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false)
    {
        return &GetMapContainingEntity(handle, true).at(handle);
    }

    void ECS::ClearSystems()
    {
        m_active.clear();
        m_toDestroy.clear();
        handleManager.Reset();
        m_transformSystem->Clear();
    }

    void ECS::RemoveTransform(ComponentHandle handle)
    {
        m_transformSystem->Remove(handle);
    }
}