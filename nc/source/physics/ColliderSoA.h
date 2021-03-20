#pragma once

#include "component/Collider.h"
#include "CollisionUtility.h"

#include <concepts>
#include <cstdint>
#include <vector>

namespace nc::physics
{
    /** SoA representation of active Colliders for CollisionSystem consumption */
    class ColliderSoA
    {
        public:
            ColliderSoA(size_t maxColliders);

            auto GetHandles() const noexcept -> const std::vector<EntityHandle::Handle_t>&;
            auto GetLayers() const noexcept -> const std::vector<physics::Layer>&;
            auto GetTransforms() const noexcept -> const std::vector<const DirectX::XMMATRIX*>&;
            auto GetVolumeProperties() const noexcept -> const std::vector<VolumeProperties>&;
            auto GetTypes() const noexcept -> const std::vector<ColliderType>&;

            void Add(EntityHandle handle, const ColliderInfo& info);
            void Remove(EntityHandle handle);
            void Clear();

            void CalculateEstimates(std::vector<DynamicEstimate>* out);

        private:
            // Collider Data
            std::vector<EntityHandle::Handle_t> m_handles;
            std::vector<physics::Layer> m_layers;
            std::vector<const DirectX::XMMATRIX*> m_transforms;
            std::vector<VolumeProperties> m_volumeProperties;
            std::vector<ColliderType> m_types;

            // Bookkeeping
            std::vector<uint32_t> m_gaps;
            uint32_t m_nextFree;
    };
} // namespace nc::physics