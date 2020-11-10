#include "ECSImpl.h"
#include "ECS.h"
#include "LightSystem.h"
#include "RenderingSystem.h"
#include "TransformSystem.h"

namespace nc::ecs
{
ECSImpl::ECSImpl(LightSystem* light, RenderingSystem* rend, TransformSystem* trans)
    : m_active{},
      m_toDestroy{},
      m_lightSystem{ light },
      m_renderingSystem{ rend },
      m_transformSystem{ trans }
{
}

void ECSImpl::SendFrameUpdate(float dt)
{
    for(auto& pair : m_active)
    {
        pair.second.SendFrameUpdate(dt);
    }
}

void ECSImpl::SendFixedUpdate()
{
    for(auto& pair : m_active)
    {
        pair.second.SendFixedUpdate();
    }
}

void ECSImpl::SendOnDestroy()
{
    for(auto & pair : m_toDestroy)
    {
        Entity* entityPtr = GetEntityPtrFromAnyMap(pair.second.Handle);
        if (entityPtr == nullptr)
        {
            continue;
        }

        pair.second.SendOnDestroy();
        const auto& handles = entityPtr->Handles;
        m_transformSystem->Remove(handles.transform);
        m_renderingSystem->Remove(handles.renderer);
        m_lightSystem->Remove(handles.pointLight);
    }
    m_toDestroy.clear();
}


bool ECSImpl::DoesEntityExist(const EntityHandle handle) noexcept
{
    return m_active.count(handle) > 0;
}

std::unordered_map<EntityHandle, Entity> & ECSImpl::GetMapContainingEntity(const EntityHandle handle, bool checkAll) noexcept(false)
{
    if (m_active.count(handle) > 0)
        return m_active;

    if (checkAll && (m_toDestroy.count(handle) > 0) ) //only check toDestroy if checkAll flag is set
        return m_toDestroy;

    throw std::runtime_error("Entity not found.");
}

std::unordered_map<EntityHandle, Entity> & ECSImpl::GetActiveEntities() noexcept
{
    return m_active;
}

std::unordered_map<EntityHandle, Entity> & ECSImpl::GetToDestroyEntities() noexcept
{
    return m_toDestroy;
}

Entity * ECSImpl::GetEntityPtrFromAnyMap(const EntityHandle handle) noexcept(false)
{
    return &GetMapContainingEntity(handle, true).at(handle);
}

void ECSImpl::ClearState()
{
    for(const auto& pair : m_active)
    {
        ECS::DestroyEntity(pair.first);
    }

    SendOnDestroy();
    m_active.clear();
    m_toDestroy.clear();
    m_handleManager.Reset();
    m_transformSystem->Clear();
    m_renderingSystem->Clear();
    m_lightSystem->Clear();
}
} // end namespace nc::ecs