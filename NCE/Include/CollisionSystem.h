#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include <vector>
#include "NCE.h"

namespace nc::internal{

    class Engine;

    class CollisionSystem
    {
        public:
            CollisionSystem();
            void CheckCollisions(const std::vector<Transform>& transforms);

        private:
            bool AreIntersecting(const Vector4& first, const Vector4& second);
    };

} //end namespace nc::internal


#endif