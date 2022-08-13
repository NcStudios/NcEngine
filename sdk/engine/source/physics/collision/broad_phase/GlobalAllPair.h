#pragma once

#include "physics/PhysicsPipelineTypes.h"
#include "physics/collision/IntersectionQueries.h"

#include <algorithm>

namespace nc::physics
{

/** Simple broad phase comparing all objects. */
template<ProxyCache ProxyCacheType>
class GlobalAllPair
{
    public:
        using proxy_cache_type = ProxyCacheType;
        using proxy_type = proxy_cache_type::proxy_type;

        void Update(proxy_cache_type* cache);
        void FindPairs();
        void Clear();

        auto PhysicsPairs() const -> std::span<const BroadPair<proxy_type>> { return m_results.physicsPairs; }
        auto TriggerPairs() const -> std::span<const BroadPair<proxy_type>> { return m_results.triggerPairs; }

    private:
        std::span<proxy_type> m_proxies;
        BroadResults<proxy_type> m_results;
};

template<ProxyCache ProxyCacheType>
void GlobalAllPair<ProxyCacheType>::Update(proxy_cache_type* cache)
{
    m_proxies = cache->Proxies();
}

template<ProxyCache ProxyCacheType>
void GlobalAllPair<ProxyCacheType>::Clear()
{
}

template<ProxyCache ProxyCacheType>
void GlobalAllPair<ProxyCacheType>::FindPairs()
{
    m_results.physicsPairs.clear();
    m_results.triggerPairs.clear();

    const size_t proxyCount = m_proxies.size();
    for(size_t i = 0u; i < proxyCount; ++i)
    {
        auto& first = m_proxies[i];

        for(size_t j = i + 1; j < proxyCount; ++j)
        {
            auto& second = m_proxies[j];

            //if(first->interactionType != ColliderInteractionType::Collider && second->interactionType != ColliderInteractionType::Collider)
            //    continue;

            // if constexpr(physics::EnableSleeping)
            // {
            //     if(!first.isAwake && !second.isAwake)
            //         continue;
            // }

            auto interactionType = GetEventType(first.InteractionType(), second.InteractionType());
            if(interactionType == CollisionEventType::None)
                continue;

            if(Intersect(first.Estimate(), second.Estimate()))
            {
                if(interactionType == CollisionEventType::Trigger)
                    m_results.triggerPairs.emplace_back(&first, &second, interactionType);
                else
                    m_results.physicsPairs.emplace_back(&first, &second, interactionType);
            }
        }
    }
}

} // namespace nc::physics
