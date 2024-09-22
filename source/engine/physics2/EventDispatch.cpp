#include "EventDispatch.h"
#include "jolt/ContactListener.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/physics/EventListeners.h"

namespace
{
void Dispatch(auto&& memberCallback,
              nc::Entity target,
              nc::Entity other,
              const nc::physics::HitInfo& hit,
              nc::ecs::Ecs& world,
              nc::ecs::ComponentPool<nc::physics::CollisionListener>& pool)
{
    if (target.ReceivesCollisionEvents() && pool.Contains(target))
    {
        auto& listener = pool.Get(target);
        if (listener.*memberCallback)
        {
            (listener.*memberCallback)(target, other, hit, world);
        }
    }
}

void Dispatch(auto&& memberCallback,
              nc::Entity target,
              nc::Entity other,
              nc::ecs::Ecs& world,
              nc::ecs::ComponentPool<nc::physics::CollisionListener>& pool)
{
    if (target.ReceivesCollisionEvents() && pool.Contains(target))
    {
        auto& listener = pool.Get(target);
        if (listener.*memberCallback)
        {
            (listener.*memberCallback)(target, other, world);
        }
    }
}

void Dispatch(auto&& callback,
              std::span<const nc::physics::CollisionPair> events,
              nc::ecs::Ecs& world,
              nc::ecs::ComponentPool<nc::physics::CollisionListener>& pool)
{
    for (const auto& [overlapping, hit] : events)
    {
        Dispatch(callback, overlapping.first, overlapping.second, hit, world, pool);
        Dispatch(callback, overlapping.second, overlapping.first, hit, world, pool);
    }
}

void Dispatch(auto&& callback,
              std::span<const nc::physics::OverlappingPair> events,
              nc::ecs::Ecs& world,
              nc::ecs::ComponentPool<nc::physics::CollisionListener>& pool)
{
    for (const auto& [_, first, second] : events)
    {
        Dispatch(callback, first, second, world, pool);
        Dispatch(callback, second, first, world, pool);
    }
}
} // anonymous namespace

namespace nc::physics
{
void DispatchPhysicsEvents(ContactListener& events, ecs::Ecs world)
{
    auto& listenerPool = world.GetPool<CollisionListener>();
    ::Dispatch(&CollisionListener::onEnter, events.GetNewCollisions(), world, listenerPool);
    ::Dispatch(&CollisionListener::onTriggerEnter, events.GetNewTriggers(), world, listenerPool);
    ::Dispatch(&CollisionListener::onExit, events.GetRemovedCollisions(), world, listenerPool);
    ::Dispatch(&CollisionListener::onTriggerExit, events.GetRemovedTriggers(), world, listenerPool);
    events.CommitPendingChanges();
}
} // namespace nc::physics
