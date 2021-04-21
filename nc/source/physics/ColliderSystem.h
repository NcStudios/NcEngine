#pragma once

#include "Ecs.h"
#include "ecs/ComponentSystem.h"
#include "ecs/SoA.h"
#include "ColliderTree.h"

#include <type_traits>

namespace nc::physics
{
    /** Wrapper around ComponentSystem<Collider> that maintains a separate
     *  representations for internal use. */
    class ColliderSystem
    {
        public:
            using DynamicColliderSoA = ecs::SoA<EntityHandle::Handle_t, const DirectX::XMMATRIX*, VolumeProperties, ColliderType>;

            ColliderSystem(uint32_t maxDynamic,
                           uint32_t maxStatic,
                           uint32_t octreeDensityThreshold,
                           float octreeMinimumExtent,
                           float worldspaceExtent);
                           
            ~ColliderSystem();

            ecs::ComponentSystem<Collider>* GetComponentSystem();
            ColliderTree* GetStaticTree();
            DynamicColliderSoA* GetDynamicSoA();

            // Wrappers around ComponentSystem methods
            Collider* Add(EntityHandle handle, const ColliderInfo& info);
            bool Remove(EntityHandle handle);
            void Clear();
            bool Contains(EntityHandle handle) const;
            Collider* GetPointerTo(EntityHandle handle);
            auto GetComponents() -> ecs::ComponentSystem<Collider>::ContainerType&;

        private:
            ecs::ComponentSystem<Collider> m_componentSystem;
            DynamicColliderSoA m_dynamicSoA;
            ColliderTree m_staticTree;
    };
} // namespace nc::physics