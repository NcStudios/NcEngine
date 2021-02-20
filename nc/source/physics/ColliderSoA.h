#pragma once

#include "component/Collider.h"

#include <concepts>
#include <cstdint>
#include <vector>

namespace nc::physics
{
    /** SoA representation of active Colliders for CollisionSystem consumption */
    struct ColliderSoA
    {
        ColliderSoA(size_t maxColliders);

        struct CenterExtentPair
        {
            DirectX::XMFLOAT3 center;
            DirectX::XMFLOAT3 extents;
        };

        // Collider Data
        std::vector<EntityHandle::Handle_t> handles;
        std::vector<const DirectX::XMMATRIX*> transforms;
        std::vector<CenterExtentPair> volumeData;
        std::vector<ColliderType> types;

        // Bookkeeping
        std::vector<uint32_t> gaps;
        uint32_t nextFree;
    };

    /** Mapping from handle to SoA instance & index */
    struct ColliderDataLocation
    {
        ColliderDataLocation();
        ColliderDataLocation(EntityHandle handle_, uint32_t index_, ColliderSoA* container_);

        EntityHandle handle;
        uint32_t index;
        ColliderSoA* container;
    };

    /** Apply a function to every valid index in a ColliderSoA */
    template<std::invocable<uint32_t> Func>
    void ForEachIndex(ColliderSoA* data, Func&& func)
    {
        uint32_t dataIndex = 0u;
        auto gapCurrent = data->gaps.cbegin();
        auto gapEnd = data->gaps.cend();

        while(dataIndex < data->nextFree)
        {
            if(gapCurrent != gapEnd && dataIndex == *gapCurrent)
            {
                ++gapCurrent; ++dataIndex; continue;
            }

            func(dataIndex++);
        }
    }
} // namespace nc::physics