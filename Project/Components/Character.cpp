#include "Character.h"

#include <iostream>

Character::Character(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle)
{
    std::cout << "creating character\n";
}

void Character::OnInitialize()
{
}

void Character::FrameUpdate()
{
    Entity* entityPtr = NCE::GetEntity(GetEntityHandle());
    Transform* transform = entityPtr->GetTransform();

    if (transform == nullptr)
    {
        std::cout << "transform ptr null\n";
        return;
    }
    transform->Translate(Vector2(input::GetXAxis(), input::GetYAxis()).GetNormalized() * m_moveSpeed * time::Time::FrameDeltaTime);
}


void Character::OnDestroy()
{

}

void Character::OnCollisionEnter(const EntityHandle other)
{

}

