#include "Character2.hpp"

Character2::Character2(Common::EntityWeakPtr parent_) : Common::Component(parent_)
{
    TypeId = 10;
}

void Character2::OnInitialize()
{
    m_transform = GetEntity().lock()->GetComponent<Components::Transform>(Components::ComponentID::ID_TRANSFORM);
}

void Character2::OnFrameUpdate()
{
    
}

void Character2::OnCollisionEnter(Common::EntityWeakPtr other_)
{
    GetEntity().lock()->DestroyEntity(GetEntity().lock()->GetID());   
}

void Character2::OnCollisionStay(Common::EntityWeakPtr other_)
{
    GetEntity().lock()->DestroyEntity(GetEntity().lock()->GetID()); 
}

void Character2::OnDestroy()
{

}

