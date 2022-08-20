#pragma once

#include "physics/PhysicsPipelineTypes.h"

#include <algorithm>
#include <cmath>
#include <span>

namespace nc::physics
{
/** Broad phase using hierarchical grids.
 *  @todo This guy needs work:
 *  - Neighboring cells are not checked. Straddling objects are just thrown up
 *    the hierarchy and eventually land in a single list.
 *  - The hierarchy could be extended to support larger objects.
 *  - Splitting FindPairs() across multiple threads provides a decent speed up. */
template<ProxyCache ProxyCacheType>
class HashedGrid
{
    public:
        using proxy_cache_type = ProxyCacheType;
        using proxy_type = proxy_cache_type::proxy_type;

        /** CellCount must be a power of two */
        static constexpr size_t LargeCellCount = 8u;
        static constexpr size_t LargeBucketCount = LargeCellCount * LargeCellCount * LargeCellCount;
        static constexpr float LargeCellSize = 20.0f;

        static constexpr size_t CellCount = 16u;
        static constexpr size_t BucketCount = CellCount * CellCount * CellCount;
        static constexpr float CellSize = 10.0f;

        HashedGrid();

        auto PhysicsPairs() const -> std::span<const BroadPair<proxy_type>> { return m_results.physicsPairs; }
        auto TriggerPairs() const -> std::span<const BroadPair<proxy_type>> { return m_results.triggerPairs; }

        void Update(proxy_cache_type* cache);
        void FindPairs();
        void Clear();

    private:
        std::vector<proxy_type*> m_buckets;
        std::vector<proxy_type*> m_largeBuckets;
        std::vector<uint32_t> m_indexMap;
        std::vector<proxy_type*> m_straddling;
        BroadResults<proxy_type> m_results;

        void FindStraddlingPairs();
        void CheckAgainstStraddling(proxy_type* proxy);
        void CheckAgainstDownstreamLinks(proxy_type* proxy);
        void CheckAgainstLarge(proxy_type* proxy, proxy_type* large);
        void Add(proxy_type* proxy);
        int HashProxy(const Sphere& sphere);
        int LargeBucketHash(const Sphere& sphere);
        auto Intersect(proxy_type* a, proxy_type* b) -> CollisionEventType;
        bool OverlapsCellBoundary(const Sphere& sphere, float cellSize);
};

template<ProxyCache ProxyCacheType>
HashedGrid<ProxyCacheType>::HashedGrid()
    : m_buckets(BucketCount, nullptr),
        m_largeBuckets(LargeBucketCount, nullptr),
        m_indexMap(),
        m_straddling{},
        m_results{}
{
    m_indexMap.reserve(BucketCount);
    size_t xPos = 0u;
    size_t yPos = 0u;
    size_t zPos = 0u;

    for(size_t i = 0; i < BucketCount; ++i)
    {
        auto x = xPos >> 1;
        auto y = yPos >> 1;
        auto z = zPos >> 1;
        auto value = x + y * LargeCellCount + z * LargeCellCount * LargeCellCount;
        m_indexMap.push_back(value);

        if(++xPos == CellCount)
        {
            xPos = 0u;
            if(++yPos == CellCount)
            {
                yPos = 0u;
                ++zPos;
            }
        }
    }
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::FindStraddlingPairs()
{
    for(size_t i = 0u; i < m_straddling.size(); ++i)
    {
        for(size_t j = i + 1; j < m_straddling.size(); ++j)
        {
            auto interactionType = Intersect(m_straddling[i], m_straddling[j]);
            if(interactionType != CollisionEventType::None)
            {
                if(interactionType == CollisionEventType::Trigger)
                    m_results.triggerPairs.emplace_back(m_straddling[i], m_straddling[j], interactionType);
                else
                    m_results.physicsPairs.emplace_back(m_straddling[i], m_straddling[j], interactionType);
            }
        }
    }
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::FindPairs()
{
    m_results.physicsPairs.clear();
    m_results.triggerPairs.clear();

    FindStraddlingPairs();

    for(size_t i = 0u; i < BucketCount; ++i)
    {
        auto* current = m_buckets[i];
        if(!current) continue;

        proxy_type* first = current;
        auto* currentLarge = m_largeBuckets[m_indexMap[i]];

        while(first)
        {
            CheckAgainstStraddling(first);
            CheckAgainstDownstreamLinks(first);
            if(currentLarge)
                CheckAgainstLarge(first, currentLarge);
            first = first->spatialData;
        }
    }

    for(size_t i = 0u; i < LargeBucketCount; ++i)
    {
        auto* current = m_largeBuckets[i];
        if(!current) continue;

        proxy_type* first = current;
        
        while(first)
        {
            CheckAgainstStraddling(first);
            CheckAgainstDownstreamLinks(first);
            first = first->spatialData;
        }
    }
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::CheckAgainstStraddling(proxy_type* proxy)
{
    for(auto* straddling : m_straddling)
    {
        auto interactionType = Intersect(proxy, straddling);
        if(interactionType != CollisionEventType::None)
        {
            if(interactionType == CollisionEventType::Trigger)
                m_results.triggerPairs.emplace_back(proxy, straddling, interactionType);
            else
                m_results.physicsPairs.emplace_back(proxy, straddling, interactionType);
        }
    }
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::CheckAgainstDownstreamLinks(proxy_type* proxy)
{
    proxy_type* next = proxy->spatialData;

    while(next)
    {
        auto interactionType = Intersect(proxy, next);
        if(interactionType != CollisionEventType::None)
        {
            if(interactionType == CollisionEventType::Trigger)
                m_results.triggerPairs.emplace_back(proxy, next, interactionType);
            else
                m_results.physicsPairs.emplace_back(proxy, next, interactionType);
        }

        next = next->spatialData;
    }
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::CheckAgainstLarge(proxy_type* proxy, proxy_type* large)
{
    while(large)
    {
        auto interactionType = Intersect(proxy, large);
        if(interactionType != CollisionEventType::None)
        {
            if(interactionType == CollisionEventType::Trigger)
                m_results.triggerPairs.emplace_back(proxy, large, interactionType);
            else
                m_results.physicsPairs.emplace_back(proxy, large, interactionType);
        }

        large = large->spatialData;
    }
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::Add(proxy_type* proxy)
{
    const auto& sphere = proxy->Estimate();
    const float radius4 = sphere.radius * 4.0f;

    if(radius4 < CellSize && !OverlapsCellBoundary(sphere, CellSize))
    {
        auto n = HashProxy(sphere);
        if(m_buckets[n] == nullptr)
        {
            m_buckets[n] = proxy;
            return;
        }

        proxy->spatialData = m_buckets[n];
        m_buckets[n] = proxy;
        return;
    }

    if(radius4 < LargeCellSize && !OverlapsCellBoundary(sphere, LargeCellSize))
    {
        auto n = LargeBucketHash(sphere);

        if(m_largeBuckets[n] == nullptr)
        {
            m_largeBuckets[n] = proxy;
            return;
        }

        proxy->spatialData = m_largeBuckets[n];
        m_largeBuckets[n] = proxy;
        return;
    }

    m_straddling.push_back(proxy);
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::Update(proxy_cache_type* cache)
{
    m_straddling.clear();
    std::ranges::fill(m_buckets, nullptr);
    std::ranges::fill(m_largeBuckets, nullptr);

    for(auto& proxy : cache->Proxies())
    {
        Add(&proxy);
    }
}

template<ProxyCache ProxyCacheType>
void HashedGrid<ProxyCacheType>::Clear()
{
    m_straddling.clear();
    std::ranges::fill(m_buckets, nullptr);
    m_results.physicsPairs.clear();
    m_results.physicsPairs.shrink_to_fit();
    m_results.triggerPairs.clear();
    m_results.triggerPairs.shrink_to_fit();
}

template<ProxyCache ProxyCacheType>
bool HashedGrid<ProxyCacheType>::OverlapsCellBoundary(const Sphere& sphere, float cellSize)
{
    auto posRelativeToCell = Vector3
    {
        std::fmod(sphere.center.x, cellSize),
        std::fmod(sphere.center.y, cellSize),
        std::fmod(sphere.center.z, cellSize)
    };

    if(posRelativeToCell.x < 0.0f)
        posRelativeToCell.x += cellSize;

    if(posRelativeToCell.y < 0.0f)
        posRelativeToCell.y += cellSize;

    if(posRelativeToCell.z < 0.0f)
        posRelativeToCell.z += cellSize;

    auto radius = Vector3::Splat(sphere.radius);
    auto max = posRelativeToCell + radius;
    auto min = posRelativeToCell - radius;

    return max.x > cellSize || max.y > cellSize || max.z > cellSize ||
            min.x < 0.0f     || min.y < 0.0f     || min.z < 0.0f;
}

template<ProxyCache ProxyCacheType>
int HashedGrid<ProxyCacheType>::HashProxy(const Sphere& sphere)
{
    auto HashDimension = [](float x)
    {
        constexpr float InverseCellSize = 1.0f / HashedGrid::CellSize;
        constexpr size_t ModuloMask = HashedGrid::CellCount - 1u;

        if(x >= 0.0f)
        {
            return static_cast<size_t>(x * InverseCellSize) & ModuloMask;
        }

        return ModuloMask - (static_cast<size_t>(-x * InverseCellSize) & ModuloMask);
    };

    auto x = HashDimension(sphere.center.x);
    auto y = HashDimension(sphere.center.y);
    auto z = HashDimension(sphere.center.z);

    return x + y * CellCount + z * CellCount * CellCount;
}

template<ProxyCache ProxyCacheType>
int HashedGrid<ProxyCacheType>::LargeBucketHash(const Sphere& sphere)
{
    auto HashDimension = [](float x)
    {
        constexpr float InverseCellSize = 1.0f / HashedGrid::LargeCellSize;
        constexpr size_t ModuloMask = HashedGrid::LargeCellCount - 1u;

        if(x >= 0.0f)
        {
            return static_cast<size_t>(x * InverseCellSize) & ModuloMask;
        }

        return ModuloMask - (static_cast<size_t>(-x * InverseCellSize) & ModuloMask);
    };

    auto x = HashDimension(sphere.center.x);
    auto y = HashDimension(sphere.center.y);
    auto z = HashDimension(sphere.center.z);

    return x + y * LargeCellCount + z * LargeCellCount * LargeCellCount;
}

template<ProxyCache ProxyCacheType>
auto HashedGrid<ProxyCacheType>::Intersect(proxy_type* current, proxy_type* next) -> CollisionEventType
{
    auto radii = current->Estimate().radius + next->Estimate().radius;
    if(SquareMagnitude(current->Estimate().center - next->Estimate().center) >= radii * radii)
    {
        return CollisionEventType::None;
    }

    return GetEventType(current->InteractionType(), next->InteractionType());
}
} // namespace nc::physics
