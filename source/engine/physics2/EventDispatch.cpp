#include "EventDispatch.h"
#include "jolt/ContactListener.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/physics/EventListeners.h"

namespace
{
void CheckDispatchOnEnter(nc::Entity target,
                          nc::Entity other,
                          const nc::physics::HitInfo& hit,
                          nc::ecs::Ecs& world,
                          nc::ecs::ComponentPool<nc::physics::CollisionListener>& pool)
{
    if (target.ReceivesCollisionEvents() && pool.Contains(target))
    {
        auto& listener = pool.Get(target);
        if (listener.onEnter)
        {
            listener.onEnter(target, other, hit, world);
        }
    }
}

void CheckDispatchOnExit(nc::Entity target,
                         nc::Entity other,
                         nc::ecs::ComponentPool<nc::physics::CollisionListener>& pool,
                         nc::ecs::Ecs& world)
{
    if (target.ReceivesCollisionEvents() && pool.Contains(target))
    {
        auto& listener = pool.Get(target);
        if (listener.onExit)
        {
            listener.onExit(target, other, world);
        }
    }
}
} // anonymous namespace

namespace nc::physics
{
void DispatchPhysicsEvents(ContactListener& events, ecs::Ecs world)
{
    auto& listenerPool = world.GetPool<CollisionListener>();
    for (const auto& [pair, hit] : events.GetAdded())
    {
        CheckDispatchOnEnter(pair.first, pair.second, hit, world, listenerPool);
        CheckDispatchOnEnter(pair.second, pair.first, hit, world, listenerPool);
    }

    for (const auto& [_, first, second] : events.GetRemoved())
    {
        CheckDispatchOnExit(first, second, listenerPool, world);
        CheckDispatchOnExit(second, first, listenerPool, world);
    }

    events.CommitPendingChanges();
}
} // namespace nc::physics
