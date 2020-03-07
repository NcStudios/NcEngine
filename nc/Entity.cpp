#include "Entity.h"
#include "NCE.h"

namespace nc{

Transform* Entity::GetTransform() const noexcept
{
    return NCE::GetTransformPtr(TransformHandle);
}

void Entity::SendOnInitialize() noexcept
{
    for (auto& comp : m_components)
    {
        comp->OnInitialize();
    }
}

void Entity::SendFrameUpdate() noexcept
{
    for (auto& comp : m_components)
    {
        comp->FrameUpdate();
    }
}

void Entity::SendFixedUpdate() noexcept
{
    for(auto& comp : m_components)
    {
        comp->FixedUpdate();
    }
}

void Entity::SendOnDestroy() noexcept
{
    for (auto& comp : m_components)
    {
        comp->OnDestroy();
    }
}

void Entity::SendOnCollisionEnter(const EntityHandle other) noexcept
{
    for(auto& comp : m_components)
    {
        comp->OnCollisionEnter(other);
    }
}

void Entity::SendOnCollisionStay() noexcept
{
    for(auto& comp : m_components)
    {
        comp->OnCollisionStay();
    }
}

void Entity::SendOnCollisionExit() noexcept
{
    for(auto& comp : m_components)
    {
        comp->OnCollisionExit();
    }
}

} //end namespace nc

