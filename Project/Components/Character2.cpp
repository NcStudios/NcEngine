#include "Character2.hpp"

Character2::Character2(Common::EntityWeakPtr t_parent) : Common::Component(t_parent)
{
    TypeId = 10;
}

void Character2::OnInitialize()
{
    _transform = GetEntity().lock()->GetComponent<Components::Transform>(Components::ComponentID::ID_TRANSFORM);
}

void Character2::OnFrameUpdate()
{
    
}

void Character2::OnCollisionEnter(Common::EntityWeakPtr t_other)
{
    GetEntity().lock()->DestroyEntity(GetEntity().lock()->GetID());   
}

void Character2::OnCollisionStay(Common::EntityWeakPtr t_other)
{
    GetEntity().lock()->DestroyEntity(GetEntity().lock()->GetID()); 
}

void Character2::OnDestroy()
{

}

