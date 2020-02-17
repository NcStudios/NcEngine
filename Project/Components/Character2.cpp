#include "Character2.h"

Character2::Character2(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle){}


void Character2::OnCollisionEnter(const EntityHandle other)
{
    Entity* otherPtr = NCE::GetEntityPtr(other);
    if (otherPtr == nullptr) 
        return;

    if (otherPtr->HasComponent<Character>())
    {
        NCE::DestroyEntity(GetEntityHandle());
    }
}

