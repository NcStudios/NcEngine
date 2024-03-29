#include "NarrowPhase.h"
#include "physics/PhysicsConstants.h"
#include "physics/PhysicsDebugging.h"
#include "ecs/Logic.h"
#include "ncutility/NcError.h"

#include "optick.h"

#include <algorithm>

namespace nc::physics
{
NarrowPhase::NarrowPhase(Registry* registry)
    : m_registry{registry},
        m_manifoldCache{},
        m_triggerCache{}
{
}

void NarrowPhase::UpdateManifolds()
{
    OPTICK_THREAD("Worker"); // one of the tasks needs to specify a thread
    /** Reverse iterate so items can be removed. */
    auto manifolds = m_manifoldCache.Data();
    auto end = manifolds.rend();
    for(auto cur = manifolds.rbegin(); cur != end; ++cur)
    {
        cur->UpdateWorldPoints(m_registry);

        /** The objects have moved apart. Remove their manifold. */
        if(cur->Contacts().empty())
        {
            auto event = cur->Event();
            m_manifoldCache.AddToRemoved(event.first, event.second);
            m_manifoldCache.Remove(event.first, event.second);
        }
    }
}

void NarrowPhase::MergeContacts(const NarrowPhysicsResult& externalResults)
{
    const auto count = externalResults.contacts.size();
    for(size_t i = 0u; i < count; ++i)
    {
        const auto& [a, b, type, sticky, unused] = externalResults.events[i];
        m_manifoldCache.Add(a, b, type, sticky, externalResults.contacts[i]);
    }
}

void NarrowPhase::CacheImpulses(std::span<const ContactConstraint> constraints)
{
    auto index = 0u;

    for(auto& manifold : m_manifoldCache.Data())
    {
        for(auto& contact : manifold.Contacts())
        {
            const auto& constraint = constraints[index];

            assert(index < constraints.size());

            contact.lambda = constraint.totalLambda;
            contact.muTangent = constraint.totalMuTangent;
            contact.muBitangent = constraint.totalMuBitangent;
            ++index;
        }
    }
}

auto NarrowPhase::TryGetCollisionLogic(Entity a) -> CollisionLogic*
{
    if(a.ReceivesCollisionEvents() && m_registry->Contains<Entity>(a))
    {
        return m_registry->Contains<CollisionLogic>(a)
            ? m_registry->Get<CollisionLogic>(a)
            : nullptr;
    }

    return nullptr;
}

void NarrowPhase::NotifyEvents()
{
    auto triggers = m_triggerCache.Data();
    auto triggerEnd = triggers.rend();
    for(auto cur = triggers.rbegin(); cur != triggerEnd; ++cur)
    {
        switch(cur->state)
        {
            [[likely]] case NarrowEvent::State::Persisting:
            {
                cur->state = NarrowEvent::State::Stale;
                break;
            }
            case NarrowEvent::State::New:
            {
                const auto e1 = cur->first;
                const auto e2 = cur->second;
                if(auto* logic = TryGetCollisionLogic(e1)) logic->NotifyTriggerEnter(e2, m_registry);
                if(auto* logic = TryGetCollisionLogic(e2)) logic->NotifyTriggerEnter(e1, m_registry);

                cur->state = NarrowEvent::State::Stale;
                break;
            }
            case NarrowEvent::State::Stale:
            {
                const auto e1 = cur->first;
                const auto e2 = cur->second;
                if(auto* logic = TryGetCollisionLogic(e1)) logic->NotifyTriggerExit(e2, m_registry);
                if(auto* logic = TryGetCollisionLogic(e2)) logic->NotifyTriggerExit(e1, m_registry);

                m_triggerCache.Remove(e1, e2);
                break;
            }
        }
    }

    auto manifolds = m_manifoldCache.Data();
    auto manifoldEnd = manifolds.rend();
    for(auto cur = manifolds.rbegin(); cur != manifoldEnd; ++cur)
    {
        switch(cur->Event().state)
        {
            [[likely]] case NarrowEvent::State::Persisting:
            {
                cur->Event().state = NarrowEvent::State::Stale;
                break;
            }
            case NarrowEvent::State::New:
            {
                const auto e1 = cur->Event().first;
                const auto e2 = cur->Event().second;
                if(auto* logic = TryGetCollisionLogic(e1)) logic->NotifyCollisionEnter(e2, m_registry);
                if(auto* logic = TryGetCollisionLogic(e2)) logic->NotifyCollisionEnter(e1, m_registry);

                cur->Event().state = NarrowEvent::State::Stale;
                break;
            }
            case NarrowEvent::State::Stale:
            {
                // No new collision this frame, but manifold found contacts still within break tolerances
                break;
            }
        }
    }

    /** Send exit events for stale and empty manifolds detected earlier in the step. */
    for(const auto& [e1, e2] : m_manifoldCache.GetRemoved())
    {
        if(auto* logic = TryGetCollisionLogic(e1)) logic->NotifyCollisionExit(e2, m_registry);
        if(auto* logic = TryGetCollisionLogic(e2)) logic->NotifyCollisionExit(e1, m_registry);
    }

    m_manifoldCache.ClearRemoved();
}

void NarrowPhase::Clear()
{
    m_manifoldCache.Clear();
    m_triggerCache.Clear();
}
} // namespace nc::physics
