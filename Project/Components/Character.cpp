#include "Character.h"

Character::Character(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle)
{
    std::cout << "creating character\n";
}

void Character::OnInitialize()
{
}

void Character::FrameUpdate()
{
    Entity* entityPtr = NCE::GetEntityPtr(GetEntityHandle());
    Transform* transform = entityPtr->GetTransform();

    if (transform == nullptr)
    {
        std::cout << "transform ptr null\n";
        return;
    }
    transform->Translate(Vector2(input::GetXAxis(), input::GetYAxis()).GetNormalized() * m_moveSpeed);
}


void Character::OnDestroy()
{

}

void Character::OnCollisionEnter(const EntityHandle other)
{

}

