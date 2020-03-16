#include "Entity.h"
#include "NCE.h"

namespace nc{

std::vector<std::shared_ptr<Component>> Entity::GetUserComponents() const noexcept
{
    return m_userComponents;
}

Transform* Entity::GetTransform() const noexcept
{
    return NCE::GetTransform(TransformHandle);
}

void Entity::SendOnInitialize() noexcept
{
    for (auto& comp : m_userComponents)
    {
        comp->OnInitialize();
    }
}

void Entity::SendFrameUpdate(float dt) noexcept
{
    for (auto& comp : m_userComponents)
    {
        comp->FrameUpdate(dt);
    }
}

void Entity::SendFixedUpdate() noexcept
{
    for(auto& comp : m_userComponents)
    {
        comp->FixedUpdate();
    }
}

void Entity::SendOnDestroy() noexcept
{
    for (auto& comp : m_userComponents)
    {
        comp->OnDestroy();
    }
}

void Entity::SendOnCollisionEnter(const EntityHandle other) noexcept
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionEnter(other);
    }
}

void Entity::SendOnCollisionStay() noexcept
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionStay();
    }
}

void Entity::SendOnCollisionExit() noexcept
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionExit();
    }
}

} //end namespace nc

