#pragma once
#include "ComponentSystem.h"
#include "component/Transform.h"

namespace nc::engine
{
    class TransformSystem : public ComponentSystem<Transform>
    {
        public:
            TransformSystem() = default;
            ~TransformSystem() = default;
            TransformSystem(TransformSystem&&) = default;
            TransformSystem& operator=(TransformSystem&&) = default;
            TransformSystem(const TransformSystem&) = delete;
            TransformSystem& operator=(const TransformSystem&) = delete;
        
        private:

    };
}
