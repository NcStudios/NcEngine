#include "Entity.h"

namespace nc
{
Entity::Entity(const EntityHandle handle, const std::string& tag) noexcept
    : Handle{ handle }, 
      Tag{ tag },
      m_userComponents{}
{
}

const std::vector<std::unique_ptr<Component>> & Entity::GetUserComponents() const noexcept
{
    return m_userComponents;
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

void Entity::SendOnCollisionEnter(Entity* other) noexcept
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionEnter(other);
    }
}

void Entity::SendOnCollisionStay(Entity* other) noexcept
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionStay(other);
    }
}

void Entity::SendOnCollisionExit(Entity* other) noexcept
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionExit(other);
    }
}
} //end namespace nc

