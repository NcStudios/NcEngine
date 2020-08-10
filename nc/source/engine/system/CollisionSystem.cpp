#include "CollisionSystem.h"

namespace nc::engine::system
{

CollisionSystem::CollisionSystem()
{
}

bool CollisionSystem::AreIntersecting(const Vector4& first, const Vector4& second)
{
    (void)first;
    (void)second;
    return true;
}

void CollisionSystem::CheckCollisions(const std::vector<Transform>& transforms)
{
    (void)transforms;
}

} //end namespace nc::engine
