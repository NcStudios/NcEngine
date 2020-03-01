#pragma once
#include <vector>

namespace nc { class Transform; class Vector4; }

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