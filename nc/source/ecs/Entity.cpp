#include "entity/Entity.h"

#include <algorithm>

namespace nc
{
Entity::Entity(EntityHandle handle, std::string tag, bool isStatic) noexcept
    : Handle{ handle }, 
      Tag{ std::move(tag) },
      IsStatic{isStatic},
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

