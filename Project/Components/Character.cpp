#include "Character.hpp"

Character::Character(EntityWeakPtr parent_) : Component(parent_)
{   
    TypeId = 10;
}

void Character::OnInitialize()
{
    m_transform = GetEntity().lock()->GetComponent<Transform>(ID_TRANSFORM);
}

void Character::OnFrameUpdate()
{
    auto t = m_transform.lock();
    if (t)
    {
        Vector2 axis(Input::GetXAxis(), Input::GetYAxis());
        axis = axis.GetNormalized() * _moveSpeed; 
        t->Translate((axis));
    }  
}


void Character::OnDestroy()
{

}

void Character::OnCollisionEnter(EntityWeakPtr t_other)
{
    //std::cout << "Character collision enter\n";
}

