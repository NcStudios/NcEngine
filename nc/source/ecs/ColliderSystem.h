#pragma once

#include "SoA.h"
#include "ColliderTree.h"
#include "assets/HullColliderManager.h"
#include "physics/CollisionVolumes.h"

namespace nc::ecs
{
    /** Wrapper around ComponentSystem<Collider> that maintains a separate
     *  representations for internal use. */
    class ColliderSystem
    {
        public:
            using DynamicColliderSoA = SoA<EntityTraits::underlying_type,
                                           DirectX::XMMATRIX,
                                           physics::BoundingVolume,
                                           ColliderType>;

            static constexpr size_t HandleTypeIndex = 0u;
            static constexpr size_t MatrixIndex = 1u;
            static constexpr size_t BoundingVolumeIndex = 2u;
            static constexpr size_t ColliderTypeIndex = 3u;

            ColliderSystem(registry_type* registry,
                           uint32_t maxDynamic,
                           uint32_t maxStatic,
                           uint32_t octreeDensityThreshold,
                           float octreeMinimumExtent,
                           float worldspaceExtent);
                           
            ~ColliderSystem();

            ColliderTree* GetStaticTree();
            DynamicColliderSoA* GetDynamicSoA();

            void Add(const Collider& collider);
            void Remove(Entity entity);
            void Clear();

        private:
            DynamicColliderSoA m_dynamicSoA;
            ColliderTree m_staticTree;
            HullColliderManager m_hullColliderManager;
    };
} // namespace nc::physics