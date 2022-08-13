#pragma once

#include "Radix.h"
#include "physics/PhysicsPipelineTypes.h"

#include <algorithm>

namespace nc::physics
{

/** Broad phase for simple pruning along a single axis. It maintains a sorted list
 *  built from a coherence-exploiting radix sort. */
template<ProxyCache ProxyCacheType>
class SingleAxisPrune
{
    struct Entry
    {
        Sphere estimate;
        float min;
        float max;
    };

    public:
        using proxy_cache_type = ProxyCacheType;
        using proxy_type = proxy_cache_type::proxy_type;

        enum class Axis { X, Y, Z };

        template<Axis SortAxis = Axis::X>
        void Update(proxy_cache_type* cache);
        void FindPairs();
        void Clear();
        auto PhysicsPairs() const -> std::span<const BroadPair<proxy_type>> { return m_results.physicsPairs; }
        auto TriggerPairs() const -> std::span<const BroadPair<proxy_type>> { return m_results.triggerPairs; }

    private:
        BroadResults<proxy_type> m_results;
        std::span<proxy_type> m_proxies;
        std::vector<Entry> m_sortedEntries;
        RadixSort m_sort;
        unsigned* m_mapToUnsorted;

        template<Axis SortAxis>
        float GetMin(const Sphere& estimate);

        template<Axis SortAxis>
        float GetMax(const Sphere& estimate);
};

template<ProxyCache ProxyCacheType>
template<SingleAxisPrune<ProxyCacheType>::Axis SortAxis>
void SingleAxisPrune<ProxyCacheType>::Update(ProxyCacheType* cache)
{
    m_results.physicsPairs.clear();
    m_results.triggerPairs.clear();
    m_proxies = cache->Proxies();

    const auto count = m_proxies.size();
    if(count == 0u) return;

    /** Extract each estimate's minimum value along the specified axis. */
    std::vector<float> minimums(count + 1, FLT_MAX);
    for(unsigned i = 0u; i < count; ++i)
    {
        const auto& estimate = m_proxies[i].Estimate();
        minimums[i] = GetMin<SortAxis>(estimate);
    }

    /** Sort the minimums */
    m_mapToUnsorted = m_sort.Sort(minimums.data(), static_cast<int>(count + 1)).GetRanks();
    m_sortedEntries.clear();
    m_sortedEntries.reserve(count + 1);

    /** Create a sorted list of estimates with min/max precomputed. */
    for(unsigned i = 0u; i < count; ++i)
    {
        const auto& estimate = m_proxies[m_mapToUnsorted[i]].Estimate();
        m_sortedEntries.emplace_back(estimate, GetMin<SortAxis>(estimate), GetMax<SortAxis>(estimate));
    }

    /** Sentinel */
    m_sortedEntries.emplace_back(Sphere{}, FLT_MAX, FLT_MAX);
}

template<ProxyCache ProxyCacheType>
void SingleAxisPrune<ProxyCacheType>::FindPairs()
{
    const unsigned count = static_cast<unsigned>(m_proxies.size());
    unsigned next = 0;
    unsigned iSorted = 0;

    while(next < count && iSorted < count)
    {
        const auto& entry = m_sortedEntries[iSorted];
        const float minLimit = entry.min;

        while(m_sortedEntries[next++].min < minLimit);

        unsigned jSorted = next;
        const float maxLimit = entry.max;
        const unsigned iUnsorted = m_mapToUnsorted[iSorted];

        while(m_sortedEntries[jSorted].min <= maxLimit)
        {
            if(Intersect(m_sortedEntries[iSorted].estimate, m_sortedEntries[jSorted].estimate))
            {
                auto& a = m_proxies[iUnsorted];
                auto& b = m_proxies[m_mapToUnsorted[jSorted]];
                const auto eventType = a.Properties().EventType(b.Properties());
                if(eventType != CollisionEventType::None)
                {
                    if(eventType == CollisionEventType::Trigger)
                        m_results.triggerPairs.emplace_back(&a, &b, eventType);
                    else
                        m_results.physicsPairs.emplace_back(&a, &b, eventType);
                }
            }

            ++jSorted;
        }

        ++iSorted;
    }
}

template<ProxyCache ProxyCacheType>
void SingleAxisPrune<ProxyCacheType>::Clear()
{
    m_results.physicsPairs.clear();
    m_results.physicsPairs.shrink_to_fit();
    m_results.triggerPairs.clear();
    m_results.triggerPairs.shrink_to_fit();
    m_sortedEntries.clear();
    m_sortedEntries.shrink_to_fit();
}

template<ProxyCache ProxyCacheType>
template<SingleAxisPrune<ProxyCacheType>::Axis SortAxis>
float SingleAxisPrune<ProxyCacheType>::GetMin(const Sphere& estimate)
{
    if constexpr(SortAxis == SingleAxisPrune<ProxyCacheType>::Axis::X)
        return estimate.center.x - estimate.radius;
    else if constexpr(SortAxis == SingleAxisPrune<ProxyCacheType>::Axis::Y)
        return estimate.center.y - estimate.radius;
    else
        return estimate.center.z - estimate.radius;
}

template<ProxyCache ProxyCacheType>
template<SingleAxisPrune<ProxyCacheType>::Axis SortAxis>
float SingleAxisPrune<ProxyCacheType>::GetMax(const Sphere& estimate)
{
    if constexpr(SortAxis == SingleAxisPrune<ProxyCacheType>::Axis::X)
        return estimate.center.x + estimate.radius;
    else if constexpr(SortAxis == SingleAxisPrune<ProxyCacheType>::Axis::Y)
        return estimate.center.y + estimate.radius;
    else
        return estimate.center.z + estimate.radius;
}

} // namespace nc::physics
