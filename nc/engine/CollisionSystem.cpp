#include "CollisionSystem.h"
//#include "NCE.h"
//#include <iostream>

namespace nc::engine{

CollisionSystem::CollisionSystem() {}

bool CollisionSystem::AreIntersecting(const Vector4& first, const Vector4& second)
{
    (void)first;
    (void)second;
    // if ((  first.X() > (second.X() + second.Z()) ) || //first.left > second.right
    //     (  first.Y() > (second.Y() + second.W()) ) || //first.top  > second.bottom
    //     ( (first.X() +  first.Z()) < second.X()  ) || //first.right < second.left
    //     ( (first.Y() +  first.W()) < second.Y()  ))   //first.bottom < second.top
    // {
    //     return false;
    // }

    return true;
}

void CollisionSystem::CheckCollisions(const std::vector<Transform>& transforms)
{
    (void)transforms;
    //int numberTransforms = transforms.size();

    // for (int i = 0; i < numberTransforms; ++i)
    // {
    //     for (int j = i + 1; j < numberTransforms; ++j)
    //     {
    //         if (!transforms.at(i).IsPhysicsEnabled() || !transforms.at(j).IsPhysicsEnabled())
    //             continue;
    //         if (!AreIntersecting(transforms.at(i).GetRect(), transforms.at(j).GetRect())) 
    //             continue;

    //         EntityHandle firstHandle = transforms.at(i).GetEntityHandle();
    //         EntityHandle secondHandle = transforms.at(j).GetEntityHandle();

    //         Entity* entity = NCE::GetEntity(firstHandle);
    //         if (entity == nullptr)
    //         {
    //             std::cout << "CollisionSystem::CheckCollisions - 1st entity ptr null" << std::endl;
    //             continue;
    //         }

    //         entity->SendOnCollisionEnter(secondHandle);
            
    //         entity = NCE::GetEntity(secondHandle);
    //         if (entity == nullptr)
    //         {
    //             std::cout << "CollisionSystem::CheckCollisions - 2nd entity ptr null" << std::endl;
    //             continue;
    //         }
    //         entity->SendOnCollisionEnter(firstHandle);
    //     }
    // }
}
} //end namespace nc::internal
