#pragma once

#include "Ecs.h"
#include "ComponentSystem.h"
#include "SoA.h"
#include "ColliderTree.h"

#include <type_traits>

namespace nc::ecs
{
    /** Wrapper around ComponentSystem<Collider> that maintains a separate
     *  representations for internal use. */
    class ColliderSystem
    {
        public:
            //using DynamicColliderSoA = SoA<HandleTraits::handle_type, const DirectX::XMMATRIX*, physics::VolumeProperties, ColliderType>;
            using DynamicColliderSoA = SoA<HandleTraits::handle_type, DirectX::XMMATRIX, physics::VolumeProperties, ColliderType>;

            static constexpr size_t HandleTypeIndex = 0u;
            static constexpr size_t MatrixIndex = 1u;
            static constexpr size_t VolumePropertiesIndex = 2u;
            static constexpr size_t ColliderTypeIndex = 3u;

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
            std::span<Collider> GetComponents();

        private:
            ecs::ComponentSystem<Collider> m_componentSystem;
            DynamicColliderSoA m_dynamicSoA;
            ColliderTree m_staticTree;
    };
} // namespace nc::physics