#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include <vector>
#include "NCE.h"
#include "Vector.h"
#include "Transform.h"

namespace nc::internal{

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

} //end namespace nc::internal


#endif