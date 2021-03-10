#pragma once

#include "Ecs.h"
#include "ecs/ComponentSystem.h"
#include "ColliderSoA.h"
#include "ColliderTree.h"

namespace nc::physics
{
    /** Wrapper around ComponentSystem<Collider> that maintains a separate
     *  SoA representation of active Colliders. */
    class ColliderSystem : private ecs::ComponentSystem<Collider>
    {
        public:
            ColliderSystem(uint32_t maxDynamic, uint32_t maxStatic, float worldspaceExtent);
            ~ColliderSystem();

            ColliderTree* GetStaticTree();
            ColliderSoA* GetDynamicSOA();

            // Wrappers around ComponentSystem methods
            Collider* Add(EntityHandle handle, const ColliderInfo& info);
            bool Remove(EntityHandle handle);
            void Clear();

            // Expose what doesn't need to be wrapped
            using ComponentSystem::Contains;
            using ComponentSystem::GetPointerTo;
            using ComponentSystem::GetComponents;

        private:
            ColliderSoA m_dynamicSoA;
            ColliderTree m_staticTree;
    };
} // namespace nc::physics