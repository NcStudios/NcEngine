#include "CollisionNotification.h"

namespace
{
    using namespace nc;
    using namespace nc::physics;

    void NotifyPhysicsEnter(registry_type* registry, const NarrowEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnCollisionEnter(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnCollisionEnter(data.first);
    }

    void NotifyTriggerEnter(registry_type* registry, const NarrowEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnTriggerEnter(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnTriggerEnter(data.first);
    }

    void NotifyPhysicsExit(registry_type* registry, const NarrowEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnCollisionExit(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnCollisionExit(data.first);
    }

    void NotifyTriggerExit(registry_type* registry, const NarrowEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnTriggerExit(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnTriggerExit(data.first);
    }

    void NotifyPhysicsStay(registry_type* registry, const NarrowEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnCollisionStay(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnCollisionStay(data.first);
    }

    void NotifyTriggerStay(registry_type* registry, const NarrowEvent& data)
    {
        if(registry->Contains<Entity>(data.first))
            registry->Get<AutoComponentGroup>(data.first)->SendOnTriggerStay(data.second);
        if(registry->Contains<Entity>(data.second))
            registry->Get<AutoComponentGroup>(data.second)->SendOnTriggerStay(data.first);
    }

    void FindPhysicsExitAndStayEvents(registry_type* registry,
                                      const std::vector<NarrowEvent>& events,
                                      const std::vector<NarrowEvent>& previousEvents,
                                      std::vector<Manifold>& manifolds)
    {
        auto beg = events.cbegin();
        auto end = events.cend();

        for(const auto& prev : previousEvents)
        {
            if(end == std::find(beg, end, prev))
            {
                NotifyPhysicsExit(registry, prev);

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
            else
            {
                NotifyPhysicsStay(registry, prev);
            }
        }
    }

    void FindTriggerExitAndStayEvents(registry_type* registry,
                                      const std::vector<NarrowEvent>& events,
                                      const std::vector<NarrowEvent>& previousEvents)
    {
        auto beg = events.cbegin();
        auto end = events.cend();

        for(const auto& prev : previousEvents)
        {
            if(end == std::find(beg, end, prev))
            {
                NotifyTriggerExit(registry, prev);
            }
            else
            {
                NotifyTriggerStay(registry, prev);
            }
        }
    }

    void FindPhysicsEnterEvents(registry_type* registry, const std::vector<NarrowEvent>& events, const std::vector<NarrowEvent>& previousEvents)
    {
        auto beg = previousEvents.cbegin();
        auto end = previousEvents.cend();

        for(const auto& cur : events)
        {
            if(end == std::find(beg, end, cur))
            {
                NotifyPhysicsEnter(registry, cur);
            }
        }
    }

    void FindTriggerEnterEvents(registry_type* registry, const std::vector<NarrowEvent>& events, const std::vector<NarrowEvent>& previousEvents)
    {
        auto beg = previousEvents.cbegin();
        auto end = previousEvents.cend();

        for(const auto& cur : events)
        {
            if(end == std::find(beg, end, cur))
            {
                NotifyTriggerEnter(registry, cur);
            }
        }
    }
} // anonymous namespace

namespace nc::physics
{
    void NotifyCollisionEvents(registry_type* registry,
                               const std::vector<NarrowEvent>& physicsEvents,
                               const std::vector<NarrowEvent>& triggerEvents,
                               CollisionCache* cache)
    {
        FindPhysicsExitAndStayEvents(registry, physicsEvents, cache->previousPhysics, cache->manifolds);
        FindPhysicsEnterEvents(registry, physicsEvents, cache->previousPhysics);
        FindTriggerExitAndStayEvents(registry, triggerEvents, cache->previousTrigger);
        FindTriggerEnterEvents(registry, triggerEvents, cache->previousTrigger);
    }
}