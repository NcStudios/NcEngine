#include "ColliderSystem.h"

namespace nc::physics
{
    ColliderSystem::ColliderSystem()
        : m_staticColliders{std::make_unique<ecs::ComponentSystem<Collider>>()},
            m_dynamicColliders{std::make_unique<ecs::ComponentSystem<Collider>>()}
    {
    }

    ecs::ComponentSystem<Collider>* ColliderSystem::GetDynamicSystem()
    {
        return m_dynamicColliders.get();
    }
    
    ecs::ComponentSystem<Collider>* ColliderSystem::GetStaticSystem()
    {
        return m_staticColliders.get();
    }

    Collider* ColliderSystem::Add(EntityHandle handle, ColliderInfo info)
    {
        if(GetEntity(handle)->IsStatic)
            return m_staticColliders->Add(handle, info);
        
        return m_dynamicColliders->Add(handle, info);
    }

    bool ColliderSystem::Remove(EntityHandle handle)
    {
        if(m_dynamicColliders->Remove(handle))
            return true;
        
        return m_staticColliders->Remove(handle);
    }

    bool ColliderSystem::Contains(EntityHandle handle) const
    {
        if(m_dynamicColliders->Contains(handle))
            return true;
        
        return m_staticColliders->Contains(handle);
    }

    Collider* ColliderSystem::GetPointerTo(EntityHandle handle) const
    {
        if(auto ptr = m_dynamicColliders->GetPointerTo(handle))
            return ptr;
        
        return m_staticColliders->GetPointerTo(handle);
    }

    void ColliderSystem::ClearState()
    {
        m_staticColliders->Clear();
        m_dynamicColliders->Clear();
    }
} // namespace nc::physics