#include "NarrowPhase.h"
#include "debug/Utils.h"
#include "optick/optick.h"

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
            if(cur->contacts.empty())
            {
                m_manifoldCache.AddToRemoved(cur->event.first, cur->event.second);
                m_manifoldCache.Remove(cur->event.first, cur->event.second);
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

        /** @todo There are some questions surrounding this:
         *  - If a manifold isn't 'updated' I would expect UpdateManfiolds() to have removed it.
         *    Why isn't this the case?
         *  - Does this actually affect stability at all? It is hard to see a difference. */
        auto manifolds = m_manifoldCache.Data();
        auto end = manifolds.rend();
        for(auto cur = manifolds.rbegin(); cur != end; ++cur)
        {
            if(cur->event.state == NarrowEvent::State::Stale)
            {
                m_manifoldCache.AddToRemoved(cur->event.first, cur->event.second);
                m_manifoldCache.Remove(cur->event.first, cur->event.second);
            }
        }
    }

    void NarrowPhase::CacheImpulses(std::span<const ContactConstraint> constraints)
    {
        auto index = 0u;

        for(auto& manifold : m_manifoldCache.Data())
        {
            for(auto& contact : manifold.contacts)
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
                    if(e1.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e1))
                        m_registry->Get<AutoComponentGroup>(e1)->SendOnTriggerEnter(e2);
                    if(e2.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e2))
                        m_registry->Get<AutoComponentGroup>(e2)->SendOnTriggerEnter(e1);

                    cur->state = NarrowEvent::State::Stale;
                    break;
                }
                case NarrowEvent::State::Stale:
                {
                    const auto e1 = cur->first;
                    const auto e2 = cur->second;
                    if(e1.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e1))
                        m_registry->Get<AutoComponentGroup>(e1)->SendOnTriggerExit(e2);
                    if(e2.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e2))
                        m_registry->Get<AutoComponentGroup>(e2)->SendOnTriggerExit(e1);

                    m_triggerCache.Remove(e1, e2);
                    break;
                }
            }
        }

        auto manifolds = m_manifoldCache.Data();
        auto manifoldEnd = manifolds.rend();
        for(auto cur = manifolds.rbegin(); cur != manifoldEnd; ++cur)
        {
            switch(cur->event.state)
            {
                [[likely]] case NarrowEvent::State::Persisting:
                {
                    cur->event.state = NarrowEvent::State::Stale;
                    break;
                }
                case NarrowEvent::State::New:
                {
                    const auto e1 = cur->event.first;
                    const auto e2 = cur->event.second;
                    if(e1.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e1))
                        m_registry->Get<AutoComponentGroup>(e1)->SendOnCollisionEnter(e2);
                    if(e2.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e2))
                        m_registry->Get<AutoComponentGroup>(e2)->SendOnCollisionEnter(e1);

                    cur->event.state = NarrowEvent::State::Stale;
                    break;
                }
                [[unlikely]] case NarrowEvent::State::Stale:
                {
                    /** This isn't expected to be detected here, but just in case. */
                    const auto e1 = cur->event.first;
                    const auto e2 = cur->event.second;
                    if(e1.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e1))
                        m_registry->Get<AutoComponentGroup>(e1)->SendOnCollisionExit(e2);
                    if(e2.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e2))
                        m_registry->Get<AutoComponentGroup>(e2)->SendOnCollisionExit(e1);

                    m_manifoldCache.Remove(e1, e2);
                    break;
                }
            }
        }

        /** Send exit events for stale and empty manifolds detected earlier in the step. */
        for(const auto& [e1, e2] : m_manifoldCache.GetRemoved())
        {
            if(e1.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e1))
                m_registry->Get<AutoComponentGroup>(e1)->SendOnCollisionExit(e2);
            if(e2.ReceivesCollisionEvents() && m_registry->Contains<Entity>(e2))
                m_registry->Get<AutoComponentGroup>(e2)->SendOnCollisionExit(e1);
        }

        m_manifoldCache.ClearRemoved();
    }

    void NarrowPhase::Clear()
    {
        m_manifoldCache.Clear();
        m_triggerCache.Clear();
    }
}