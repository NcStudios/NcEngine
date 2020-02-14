#ifndef COLLISION_SYSTEM
#define COLLISION_SYSTEM

#include <vector>
#include "NCE.h"
#include "Vector.h"
#include "Transform.h"

class Engine;

class CollisionSystem
{
    private:
        Engine* m_engine; //better way?

        bool AreIntersecting(const Vector4& first, const Vector4& second);

    public:
        CollisionSystem(Engine* enginePtr);

        void CheckCollisions(const std::vector<Transform>& transforms);
};


#endif