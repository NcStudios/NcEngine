#include "NarrowPhase.h"
#include "debug/Utils.h"
#include "debug/Profiler.h"

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
        auto manifolds = m_manifoldCache.GetData();
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
        auto manifolds = m_manifoldCache.GetData();
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

        for(auto& manifold : m_manifoldCache.GetData())
        {
            for(auto& contact : manifold.contacts)
            {
                const auto& constraint = constraints[index];

                assert(index < m_contactConstraints.size());
                assert(manifold.event.first == constraint.entityA && manifold.event.second == constraint.entityB);

                contact.lambda = constraint.totalLambda;
                contact.muTangent = constraint.totalMuTangent;
                contact.muBitangent = constraint.totalMuBitangent;
                ++index;
            }
        }
    }

    void NarrowPhase::NotifyEvents()
    {
        auto triggers = m_triggerCache.GetData();
        auto triggerEnd = triggers.rend();
        for(auto cur = triggers.rbegin(); cur != triggerEnd; ++cur)
        {
            switch(cur->state)
            {
                case NarrowEvent::State::New:
                {
                    if(m_registry->Contains<Entity>(cur->first))
                        m_registry->Get<AutoComponentGroup>(cur->first)->SendOnTriggerEnter(cur->second);
                    if(m_registry->Contains<Entity>(cur->second))
                        m_registry->Get<AutoComponentGroup>(cur->second)->SendOnTriggerEnter(cur->first);

                    cur->state = NarrowEvent::State::Stale;
                    break;
                }
                case NarrowEvent::State::Persisting:
                {
                    if(m_registry->Contains<Entity>(cur->first))
                        m_registry->Get<AutoComponentGroup>(cur->first)->SendOnTriggerStay(cur->second);
                    if(m_registry->Contains<Entity>(cur->second))
                        m_registry->Get<AutoComponentGroup>(cur->second)->SendOnTriggerStay(cur->first);

                    cur->state = NarrowEvent::State::Stale;
                    break;
                }
                case NarrowEvent::State::Stale:
                {
                    if(m_registry->Contains<Entity>(cur->first))
                        m_registry->Get<AutoComponentGroup>(cur->first)->SendOnTriggerExit(cur->second);
                    if(m_registry->Contains<Entity>(cur->second))
                        m_registry->Get<AutoComponentGroup>(cur->second)->SendOnTriggerExit(cur->first);

                    m_triggerCache.Remove(cur->first, cur->second);
                    break;
                }
            }
        }

        auto manifolds = m_manifoldCache.GetData();
        auto manifoldEnd = manifolds.rend();
        for(auto cur = manifolds.rbegin(); cur != manifoldEnd; ++cur)
        {
            switch(cur->event.state)
            {
                [[likely]] case NarrowEvent::State::Persisting:
                {
                    if(m_registry->Contains<Entity>(cur->event.first))
                        m_registry->Get<AutoComponentGroup>(cur->event.first)->SendOnCollisionStay(cur->event.second);
                    if(m_registry->Contains<Entity>(cur->event.second))
                        m_registry->Get<AutoComponentGroup>(cur->event.second)->SendOnCollisionStay(cur->event.first);

                    cur->event.state = NarrowEvent::State::Stale;
                    break;
                }
                case NarrowEvent::State::New:
                {
                    if(m_registry->Contains<Entity>(cur->event.first))
                        m_registry->Get<AutoComponentGroup>(cur->event.first)->SendOnCollisionEnter(cur->event.second);
                    if(m_registry->Contains<Entity>(cur->event.second))
                        m_registry->Get<AutoComponentGroup>(cur->event.second)->SendOnCollisionEnter(cur->event.first);

                    cur->event.state = NarrowEvent::State::Stale;
                    break;
                }
                [[unlikely]] case NarrowEvent::State::Stale:
                {
                    /** This isn't expected to be detected here, but just in case. */
                    if(m_registry->Contains<Entity>(cur->event.first))
                        m_registry->Get<AutoComponentGroup>(cur->event.first)->SendOnCollisionExit(cur->event.second);
                    if(m_registry->Contains<Entity>(cur->event.second))
                        m_registry->Get<AutoComponentGroup>(cur->event.second)->SendOnCollisionExit(cur->event.first);

                    m_manifoldCache.Remove(cur->event.first, cur->event.second);
                    break;
                }
            }
        }

        /** Send exit events for stale and empty manifolds detected earlier in the step. */
        for(auto& removed : m_manifoldCache.GetRemoved())
        {
            if(m_registry->Contains<Entity>(removed.first))
                m_registry->Get<AutoComponentGroup>(removed.first)->SendOnCollisionExit(removed.second);
            if(m_registry->Contains<Entity>(removed.second))
                m_registry->Get<AutoComponentGroup>(removed.second)->SendOnCollisionExit(removed.first);
        }

        m_manifoldCache.ClearRemoved();
    }

    void NarrowPhase::Clear()
    {
        m_manifoldCache.Clear();
        m_triggerCache.Clear();
    }
}