#pragma once

#include "Ecs.h"
#include "ecs/ComponentSystem.h"
#include "ColliderSoA.h"

namespace nc::physics
{
    /** Wrapper around ComponentSystem<Collider> that maintains a separate
     *  SoA representation of active Colliders. */
    class ColliderSystem : private ecs::ComponentSystem<Collider>
    {
        public:
            ColliderSystem(uint32_t maxColliders);

            ColliderSoA* GetStaticSOA();
            ColliderSoA* GetDynamicSOA();

            // Wrappers around ComponentSystem methods
            Collider* Add(EntityHandle handle, ColliderInfo info);
            bool Remove(EntityHandle handle);
            void Clear();

            // Expose what doesn't need to be wrapped
            using ComponentSystem::Contains;
            using ComponentSystem::GetPointerTo;
            using ComponentSystem::GetComponents;

        private:
            ColliderSoA m_static;
            ColliderSoA m_dynamic;
            const uint32_t m_maxColliders;
            std::vector<ColliderDataLocation> m_locations;
    };
} // namespace nc::physics