#include "CollisionNotification.h"

namespace
{
    using namespace nc;
    using namespace nc::physics;

    template<CollisionInteractionType CollisionDescription>
    auto GetNarrowEventsByType(const CollisionCache* cache) -> const std::vector<NarrowDetectEvent>&;

    template<CollisionInteractionType CollisionDescription>
    auto GetPreviousNarrowEventsByType(const CollisionCache* cache) -> const std::vector<NarrowDetectEvent>&;

    template<CollisionInteractionType CollisionDescription>
    void FindExitAndStayEvents(registry_type* registry, CollisionCache* cache);

    template<CollisionInteractionType CollisionDescription>
    void FindEnterEvents(registry_type* registry, CollisionCache* cache);

    template<CollisionInteractionType CollisionDescription>
    void NotifyEnter(registry_type* registry, const NarrowDetectEvent& data);

    template<CollisionInteractionType CollisionDescription>
    void NotifyExit(registry_type* registry, const NarrowDetectEvent& data);

    template<CollisionInteractionType CollisionDescription>
    void NotifyStay(registry_type* registry, const NarrowDetectEvent& data);

    template<CollisionInteractionType CollisionDescription>
    auto GetNarrowEventsByType(const CollisionCache* cache) -> const std::vector<NarrowDetectEvent>&
    {
        if constexpr(CollisionDescription == CollisionInteractionType::Physics)
            return cache->narrow.physics;
        else
            return cache->narrow.trigger;
    }

    template<CollisionInteractionType CollisionDescription>
    auto GetPreviousNarrowEventsByType(const CollisionCache* cache) -> const std::vector<NarrowDetectEvent>&
    {
        if constexpr(CollisionDescription == CollisionInteractionType::Physics)
            return cache->previousNarrow.physics;
        else
            return cache->previousNarrow.trigger;
    }

    template<CollisionInteractionType CollisionDescription>
    void FindExitAndStayEvents(registry_type* registry, CollisionCache* cache)
    {
        const auto& current = GetNarrowEventsByType<CollisionDescription>(cache);
        auto beg = current.cbegin();
        auto end = current.cend();

        for(const auto& prev : GetPreviousNarrowEventsByType<CollisionDescription>(cache))
        {
            if(end == std::find(beg, end, prev))
            {
                NotifyExit<CollisionDescription>(registry, prev);

                if constexpr(CollisionDescription == CollisionInteractionType::Physics)
                {
                    auto& manifolds = cache->manifolds;

                    auto pos = std::ranges::find_if(manifolds, [prev](const auto& manifold)
                    {
                        return (manifold.entityA == prev.first && manifold.entityB == prev.second) ||
                               (manifold.entityA == prev.second && manifold.entityB == prev.first);
                    });

                    if(pos != manifolds.end())
                    {
                        *pos = manifolds.back();
                        manifolds.pop_back();
                    }
                }
            }
            else
            {
                NotifyStay<CollisionDescription>(registry, prev);
            }
        }
    }

    template<CollisionInteractionType CollisionDescription>
    void FindEnterEvents(registry_type* registry, CollisionCache* cache)
    {
        const auto& previous = GetPreviousNarrowEventsByType<CollisionDescription>(cache);
        auto beg = previous.cbegin();
        auto end = previous.cend();

        for(const auto& cur : GetNarrowEventsByType<CollisionDescription>(cache))
        {
            if(end == std::find(beg, end, cur))
            {
                NotifyEnter<CollisionDescription>(registry, cur);
            }
        }
    }

    template<>
    void NotifyEnter<CollisionInteractionType::Physics>(registry_type* registry, const NarrowDetectEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnCollisionEnter(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnCollisionEnter(data.first);
    }

    template<>
    void NotifyEnter<CollisionInteractionType::Trigger>(registry_type* registry, const NarrowDetectEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnTriggerEnter(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnTriggerEnter(data.first);
    }

    template<>
    void NotifyExit<CollisionInteractionType::Physics>(registry_type* registry, const NarrowDetectEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnCollisionExit(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnCollisionExit(data.first);
    }

    template<>
    void NotifyExit<CollisionInteractionType::Trigger>(registry_type* registry, const NarrowDetectEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnTriggerExit(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnTriggerExit(data.first);
    }

    template<>
    void NotifyStay<CollisionInteractionType::Physics>(registry_type* registry, const NarrowDetectEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnCollisionStay(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnCollisionStay(data.first);
    }

    template<>
    void NotifyStay<CollisionInteractionType::Trigger>(registry_type* registry, const NarrowDetectEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnTriggerStay(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnTriggerStay(data.first);
    }
} // anonymous namespace

namespace nc::physics
{
    void NotifyCollisionEvents(registry_type* registry, CollisionCache* cache)
    {
        FindExitAndStayEvents<CollisionInteractionType::Physics>(registry, cache);
        FindEnterEvents<CollisionInteractionType::Physics>(registry, cache);
        FindExitAndStayEvents<CollisionInteractionType::Trigger>(registry, cache);
        FindEnterEvents<CollisionInteractionType::Trigger>(registry, cache);
    }
}