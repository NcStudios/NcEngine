#include "Entity.h"
#include "NcCommon.h"

namespace nc{

Entity::Entity(const EntityHandle handle, const ComponentHandle transformHandle, const std::string& tag) noexcept
    : Handle{ handle }, 
      Tag{ tag },
      Handles{ transformHandle, NullHandle, NullHandle },
      m_userComponents{ }
{
}

Entity::Entity(Entity&& other)
    : Handle { other.Handle },
      Tag { std::move(other.Tag) },
      Handles {std::exchange(other.Handles, {}) },
      m_userComponents { std::move(other.m_userComponents) }
{
}

Entity& Entity::operator=(Entity&& other)
{
    Handle = std::exchange(other.Handle, NullHandle);
    Tag = std::move(other.Tag);
    Handles = std::exchange(other.Handles, {});
    m_userComponents = std::move(other.m_userComponents);
    return *this;
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

