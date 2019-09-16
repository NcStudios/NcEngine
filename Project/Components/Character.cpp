#include "Character.hpp"

Character::Character(EntityWeakPtr t_parent) : Component(t_parent)
{   
    TypeId = 10;
}

void Character::OnInitialize()
{
    _transform = GetEntity().lock()->GetComponent<Transform>(ID_TRANSFORM);
}

void Character::OnFrameUpdate()
{
    int x = 0;
    int y = 0;

    if (Input::GetKey('W'))
        y -= _moveSpeed;
    if (Input::GetKey('S'))
        y += _moveSpeed;
    if (Input::GetKey('A'))
        x -= _moveSpeed;
    if (Input::GetKey('D'))
        x += _moveSpeed;

    if (_transform.lock() == nullptr)
    {
        std::cout << "** in: Character::UpdatePosition() _position is null" << '\n';
        return;
    }

    _transform.lock()->Translate(x, y);
}


void Character::OnDestroy()
{

}

void Character::OnCollisionEnter(EntityWeakPtr t_other)
{
    //std::cout << "Character collision enter" << '\n';
}

