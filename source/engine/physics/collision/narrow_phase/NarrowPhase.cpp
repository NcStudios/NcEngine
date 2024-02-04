#include "NarrowPhase.h"
#include "ecs/Logic.h"
#include "ncutility/NcError.h"

#include "optick.h"

#include <algorithm>

#include <iostream>

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
        const auto& [a, b, type, unused] = externalResults.events[i];
        m_manifoldCache.Add(a, b, type, externalResults.contacts[i]);
    }

    // Should get rid of this removal and the one in notify events. UpdateManifolds should correctly clear
    // itself always - wackiness should be the result of bad parameters/break distance
    /** @todo There are some questions surrounding this:
     *  - If a manifold isn't 'updated' I would expect UpdateManfiolds() to have removed it.
     *    Why isn't this the case?
     *  - Does this actually affect stability at all? It is hard to see a difference. */
    auto manifolds = m_manifoldCache.Data();
    auto end = manifolds.rend();
    for(auto cur = manifolds.rbegin(); cur != end; ++cur)
    {
        if(cur->Event().state == NarrowEvent::State::Stale)
        {
#if NC_PHYSICS_DEBUG_CONTACT_POINTS
            std::cout << "Remove stale manifold [contact not broken]\n";
            for (auto& c : cur->Contacts())
                std::cout << "\t" << c.depth << ", ";
            std::cout << '\n';
#endif
            m_manifoldCache.AddToRemoved(cur->Event().first, cur->Event().second);
            m_manifoldCache.Remove(cur->Event().first, cur->Event().second);
        }
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
        return m_registry->Get<CollisionLogic>(a);
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
            [[unlikely]] case NarrowEvent::State::Stale:
            {
#if NC_PHYSICS_DEBUG_CONTACT_POINTS
                std::cerr << "Clear stale manifold [expected impossible code path]\n";
#endif
                /** This isn't expected to be detected here, but just in case. */
                const auto e1 = cur->Event().first;
                const auto e2 = cur->Event().second;
                if(auto* logic = TryGetCollisionLogic(e1)) logic->NotifyCollisionExit(e2, m_registry);
                if(auto* logic = TryGetCollisionLogic(e2)) logic->NotifyCollisionExit(e1, m_registry);

                m_manifoldCache.Remove(e1, e2);
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
