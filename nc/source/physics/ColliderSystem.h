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
            ColliderSystem(uint32_t maxColliders);
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
            Collider* AddStatic(EntityHandle handle, const ColliderInfo& info);
            Collider* AddDynamic(EntityHandle handle, const ColliderInfo& info);
            void RemoveStatic(EntityHandle handle);
            void RemoveDynamic(uint32_t index);

            ColliderSoA m_dynamicSoA;
            ColliderTree m_staticTree;
            const uint32_t m_maxColliders;
            std::vector<ColliderDataLocation> m_locations;
    };
} // namespace nc::physics