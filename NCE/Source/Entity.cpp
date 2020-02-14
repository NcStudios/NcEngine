#include "../Include/Entity.h"

#include "../Include/NCE.h"

Transform* Entity::GetTransform()
{
    return NCE::GetTransformPtr(TransformHandle);
}

void Entity::SendOnInitialize()
{
    for (auto& comp : m_components)
    {
        comp->OnInitialize();
    }
}

void Entity::SendFrameUpdate()
{
    for (auto& comp : m_components)
    {
        comp->FrameUpdate();
    }
}

void Entity::SendFixedUpdate()
{
    for(auto& comp : m_components)
    {
        comp->FixedUpdate();
    }
}

void Entity::SendOnDestroy()
{
    for (auto& comp : m_components)
    {
        comp->OnDestroy();
    }
}

void Entity::SendOnCollisionEnter(const EntityHandle other)
{
    for(auto& comp : m_components)
    {
        comp->OnCollisionEnter(other);
    }
}

void Entity::SendOnCollisionStay()
{
    for(auto& comp : m_components)
    {
        comp->OnCollisionStay();
    }
}

void Entity::SendOnCollisionExit()
{
    for(auto& comp : m_components)
    {
        comp->OnCollisionExit();
    }
}



