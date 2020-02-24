#include "Tail.h"

Tail::Tail(ComponentHandle handle, EntityHandle parentHandle) : Component(handle, parentHandle) {}

void Tail::OnInitialize()
{

}

void Tail::FrameUpdate()
{
    Entity* thisEntity = NCE::GetEntity(GetEntityHandle());
    Transform* thisTransform = thisEntity->GetTransform();
    thisTransform->SetPosition(Vector2::Lerp(thisTransform->GetPosition(), nextPos, 0.1));
    
    if(hasTail)
    {
        NCE::GetEntity(tail)->GetComponent<Tail>()->SetNextPosition(thisTransform->GetPosition());
    }
}

void Tail::AddTail()
{
    if(hasTail)
    {
        NCE::GetEntity(tail)->GetComponent<Tail>()->AddTail();
    }
    else
    {
        hasTail = true;
        Vector4 currentRect = NCE::GetEntity(GetEntityHandle())->GetTransform()->GetRect();
        int width = currentRect.GetZ() < 6 ? 1 : currentRect.GetZ() - 1;
        int height = currentRect.GetW() < 6 ? 1 : currentRect.GetW() - 1;
        Vector4 newTailRect(currentRect.GetX(), currentRect.GetY(), width, height);
        EntityHandle newTailHandle = NCE::CreateEntity(newTailRect, true, true, "");
        Entity* newTailEntity = NCE::GetEntity(newTailHandle);
        auto newTailComponent = newTailEntity->AddComponent<Tail>();
        tail = newTailHandle;    
    }
}

void Tail::SetNextPosition(Vector2 next)
{
    nextPos = next;
}

