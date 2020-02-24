#include "Head.h"

#include <iostream>

Head::Head(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle)
{
}

void Head::OnInitialize()
{

}

void Head::FrameUpdate()
{
    Entity* entityPtr = NCE::GetEntity(GetEntityHandle());
    Transform* transform = entityPtr->GetTransform();
    transform->Translate(input::GetAxis().GetNormalized() * m_moveSpeed * time::Time::FrameDeltaTime);

    if(hasTail)
    {
        NCE::GetEntity(tail)->GetComponent<Tail>()->SetNextPosition(transform->GetPosition());
    }
}


void Head::OnDestroy()
{
}

void Head::OnCollisionEnter(const EntityHandle other)
{
    Entity* otherPtr = NCE::GetEntity(other);
    if (otherPtr && otherPtr->HasComponent<Point>()) 
    {
        if(hasTail)
        {
            NCE::GetEntity(tail)->GetComponent<Tail>()->AddTail();
        }   
        else
        {
            EntityHandle newTailHandle = NCE::CreateEntity(NCE::GetEntity(GetEntityHandle())->GetTransform()->GetRect(), true, true, "");
            Entity* newTailEntity = NCE::GetEntity(newTailHandle);
            auto newTailComponent = newTailEntity->AddComponent<Tail>();
            hasTail = true;
            tail = newTailHandle;
        } 
    }
}

