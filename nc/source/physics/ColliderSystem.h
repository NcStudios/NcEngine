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
            /** @todo either make this dynamically sized or set this from config */
            ColliderSystem(uint32_t maxColliders = 20000u);
            ~ColliderSystem();

            ColliderSoA* GetStaticSOA();
            ColliderSoA* GetDynamicSOA();

            Collider* Add(EntityHandle handle, ColliderInfo info);
            bool Remove(EntityHandle handle);
            void ClearState();

            // Expose what doesn't need to be wrapped
            using ComponentSystem::Contains;
            using ComponentSystem::GetPointerTo;
            using ComponentSystem::ForEach;

        private:
            ColliderSoA m_static;
            ColliderSoA m_dynamic;
            const uint32_t m_maxColliders;
            std::vector<IndexData> m_indexData;
    };
} // namespace nc::physics