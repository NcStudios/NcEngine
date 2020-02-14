#include "Character2.h"

Character2::Character2(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle){}

void Character2::OnInitialize()
{
}

void Character2::FrameUpdate()
{
    
}

void Character2::OnCollisionEnter(const EntityHandle other)
{
    Entity* otherPtr = NCE::GetEntityPtr(other);
    if (otherPtr == nullptr) 
        return;

    if (otherPtr->HasComponent<Character>())
    {
        std::cout << "has character component" << std::endl;
        NCE::DestroyEntity(GetEntityHandle());
    }
    else
    {
        //std::cout << "does not have character component" << std::endl;
    }
    
}

void Character2::OnCollisionStay()
{

}

void Character2::OnDestroy()
{

}

