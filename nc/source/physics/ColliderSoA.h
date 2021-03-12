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
            auto GetVolumes() const noexcept -> const std::vector<Collider::BoundingVolume>&;
            auto GetTransforms() const noexcept -> const std::vector<const DirectX::XMMATRIX*>&;
            auto GetVolumeProperties() const noexcept -> const std::vector<VolumeProperties>&;
            auto GetTypes() const noexcept -> const std::vector<ColliderType>&;

            void Add(EntityHandle handle, const ColliderInfo& info);
            void Remove(EntityHandle handle);
            void Clear();

            void CalculateEstimates(std::vector<DynamicEstimate>* out);

        private:
            // Collider Data
            std::vector<EntityHandle::Handle_t> handles;
            std::vector<Collider::BoundingVolume> volumes;
            std::vector<const DirectX::XMMATRIX*> transforms;
            std::vector<VolumeProperties> volumeProperties;
            std::vector<ColliderType> types;

            // Bookkeeping
            std::vector<uint32_t> gaps;
            uint32_t nextFree;
    };
} // namespace nc::physics