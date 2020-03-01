#include "CollisionSystem.h"
#include "NCE.h"
#include <iostream>

namespace nc::internal{

CollisionSystem::CollisionSystem() {}

bool CollisionSystem::AreIntersecting(const Vector4& first, const Vector4& second)
{
    if ((  first.GetX() > (second.GetX() + second.GetZ()) ) || //first.left > second.right
        (  first.GetY() > (second.GetY() + second.GetW()) ) || //first.top  > second.bottom
        ( (first.GetX() +  first.GetZ()) < second.GetX()  ) || //first.right < second.left
        ( (first.GetY() +  first.GetW()) < second.GetY()  ))   //first.bottom < second.top
    {
        return false;
    }

    return true;
}

void CollisionSystem::CheckCollisions(const std::vector<Transform>& transforms)
{
    int numberTransforms = transforms.size();

    for (int i = 0; i < numberTransforms; ++i)
    {
        for (int j = i + 1; j < numberTransforms; ++j)
        {
            if (!transforms.at(i).IsPhysicsEnabled() || !transforms.at(j).IsPhysicsEnabled())
                continue;
            if (!AreIntersecting(transforms.at(i).GetRect(), transforms.at(j).GetRect())) 
                continue;

            EntityHandle firstHandle = transforms.at(i).GetEntityHandle();
            EntityHandle secondHandle = transforms.at(j).GetEntityHandle();

            Entity* entity = NCE::GetEntity(firstHandle);
            if (entity == nullptr)
            {
                std::cout << "CollisionSystem::CheckCollisions - 1st entity ptr null" << std::endl;
                continue;
            }

            entity->SendOnCollisionEnter(secondHandle);
            
            entity = NCE::GetEntity(secondHandle);
            if (entity == nullptr)
            {
                std::cout << "CollisionSystem::CheckCollisions - 2nd entity ptr null" << std::endl;
                continue;
            }
            entity->SendOnCollisionEnter(firstHandle);
        }
    }
}
} //end namespace nc::internal
