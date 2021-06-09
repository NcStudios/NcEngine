#include "entity/Entity.h"

#include <algorithm>

namespace nc
{
Entity::Entity(EntityHandle handle, std::string tag, physics::Layer layer) noexcept
    : Handle{handle}, 
      Tag{std::move(tag)},
      Layer{layer},
      m_userComponents{}
{
}

std::vector<Component*> Entity::GetUserComponents() const noexcept
{
    std::vector<Component*> out(m_userComponents.size());
    std::transform(m_userComponents.cbegin(), m_userComponents.cend(), out.begin(), [](const auto& ptr)
    {
        return ptr.get();
    });
    return out;
}

void Entity::SendFrameUpdate(float dt)
{
    for (auto& comp : m_userComponents)
    {
        comp->FrameUpdate(dt);
    }
}

void Entity::SendFixedUpdate()
{
    for(auto& comp : m_userComponents)
    {
        comp->FixedUpdate();
    }
}

void Entity::SendOnDestroy()
{
    for (auto& comp : m_userComponents)
    {
        comp->OnDestroy();
    }
}

void Entity::SendOnCollisionEnter(EntityHandle hit)
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionEnter(hit);
    }
}

void Entity::SendOnCollisionStay(EntityHandle hit)
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionStay(hit);
    }
}

void Entity::SendOnCollisionExit(EntityHandle hit)
{
    for(auto& comp : m_userComponents)
    {
        comp->OnCollisionExit(hit);
    }
}
} //end namespace nc

