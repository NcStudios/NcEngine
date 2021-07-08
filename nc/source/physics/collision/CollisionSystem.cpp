#include "CollisionSystem.h"
#include "Ecs.h"
#include "IntersectionQueries.h"
#include "config/Config.h"
#include "debug/Profiler.h"
#include "job/JobSystem.h"

#ifdef NC_EDITOR_ENABLED
    #define INCREMENT_METRIC(value) ++value
#else
    #define INCREMENT_METRIC(value)
#endif

namespace nc::physics
{
    bool operator ==(const NarrowDetectEvent& lhs, const NarrowDetectEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    bool operator ==(const BroadDetectVsStaticEvent& lhs, const BroadDetectVsStaticEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second);
    }

    CollisionSystem::CollisionSystem(ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
        : m_colliderSystem{colliderSystem},
          m_jobSystem{jobSystem},
          m_dynamicEstimates{},
          m_broadEventsVsDynamic{},
          m_broadEventsVsStatic{},
          m_currentCollisions{},
          m_previousCollisions{},
          m_persistentManifolds{}
    {
    }

    auto CollisionSystem::DoCollisionStep(registry_type* registry) -> const std::vector<Manifold>&
    {
        #ifdef NC_EDITOR_ENABLED
        metrics.Reset();
        #endif

        for(auto& manifold : m_persistentManifolds)
        {
            manifold.UpdateWorldPoints(registry);
        }

        /** @todo re-visit JobSystem here */ 
        FetchEstimates(registry);
        BroadDetectVsStatic();
        BroadDetectVsDynamic();
        NarrowDetectVsStatic();
        NarrowDetectVsDynamic();

        return m_persistentManifolds;
    }

    void CollisionSystem::NotifyCollisionEvents(registry_type* registry)
    {
        FindExitAndStayEvents(registry);
        FindEnterEvents(registry);
    }

    /** End of frame/step cleanup. */
    void CollisionSystem::Cleanup()
    {
        m_previousCollisions = std::move(m_currentCollisions);
        m_currentCollisions.clear();
        m_dynamicEstimates.clear();
        m_broadEventsVsDynamic.clear();
        m_broadEventsVsStatic.clear();
    }

    /** Reset all state. */
    void CollisionSystem::ClearState()
    {
        m_broadEventsVsDynamic.resize(0u);
        m_broadEventsVsStatic.resize(0u);
        m_currentCollisions.resize(0u);
        m_previousCollisions.resize(0u);
        m_persistentManifolds.resize(0u);
    }

    /** Update locally stored transformation matrices and estimated bounding volumes
     *  for each dynamic collider. */
    void CollisionSystem::FetchEstimates(registry_type* registry)
    {
        const auto* soa = m_colliderSystem->GetDynamicSoA();
        auto [index, handles, matrices, volumes] = soa->View<ecs::ColliderSystem::HandleTypeIndex,
                                                             ecs::ColliderSystem::MatrixIndex,
                                                             ecs::ColliderSystem::BoundingVolumeIndex>();

        while(index.Valid())
        {
            matrices[index] = registry->Get<Transform>(static_cast<Entity>(handles[index]))->GetTransformationMatrix();
            m_dynamicEstimates.emplace_back(EstimateBoundingVolume(volumes[index], matrices[index]), index);
            ++index;
        }
    }

    /** Check for intersection between all dynamic volume estimates. */
    void CollisionSystem::BroadDetectVsDynamic()
    {
        const auto dynamicSize = m_dynamicEstimates.size();
        for(size_t i = 0u; i < dynamicSize; ++i)
        {
            for(size_t j = i + 1; j < dynamicSize; ++j)
            {
                INCREMENT_METRIC(metrics.dynamicBroadChecks);

                if(Intersect(m_dynamicEstimates[i].estimate, m_dynamicEstimates[j].estimate))
                    m_broadEventsVsDynamic.emplace_back(m_dynamicEstimates[i].index, m_dynamicEstimates[j].index);
            }
        }
    }

    /** Check for intersection between dynamic volume estimates and static estimates. */
    void CollisionSystem::BroadDetectVsStatic()
    {
        const auto dynamicSize = m_dynamicEstimates.size();
        auto* staticTree = m_colliderSystem->GetStaticTree();
        for(size_t i = 0u; i < dynamicSize; ++i)
        {
            for(const auto* pair : staticTree->BroadCheck(m_dynamicEstimates[i].estimate))
            {
                // Because static volumes can exist in multiple tree nodes, identical intersections may be reported.
                const auto beg = m_broadEventsVsStatic.cbegin();
                const auto end = m_broadEventsVsStatic.cend();

                auto newEvent = BroadDetectVsStaticEvent{m_dynamicEstimates[i].index, pair};
                if(end == std::find(beg, end, newEvent))
                {
                    INCREMENT_METRIC(metrics.staticBroadChecks);
                    m_broadEventsVsStatic.push_back(newEvent);
                }
            }
        }
    }

    /** Perform narrow phase collision checks on all estimated dynamic vs. dynamic collisions.
     *  For collisions between non trigger colliders, use Epa to generate contacts. */
    void CollisionSystem::NarrowDetectVsDynamic()
    {
        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<BoundingVolume>();
        const auto trigger = dynamicSoA->GetSpan<bool>();
        CollisionState state;

        for(const auto& [i, j] : m_broadEventsVsDynamic)
        {
            INCREMENT_METRIC(metrics.dynamicNarrowChecks);
            const auto& iVolume = volumes[i];
            const auto& jVolume = volumes[j];
            const auto& iMatrix = matrices[i];
            const auto& jMatrix = matrices[j];

            if(Gjk(iVolume, jVolume, iMatrix, jMatrix, &state))
            {
                INCREMENT_METRIC(metrics.dynamicCollisions);

                /** @todo We could notify based on entries in m_manifold instead
                 *  of m_currentCollisions, but that won't work for trigger colliders.
                 *  Leaving m_currentCollisions unchanged for now. */
                m_currentCollisions.emplace_back(handles[i], handles[j]);

                if(!trigger[i] && !trigger[j])
                {
                    Contact contact;
                    if(Epa(iVolume, jVolume, iMatrix, jMatrix, &state, &contact))
                    {
                        AddContact(handles[i], handles[j], contact);
                    }
                }
            }
        }
    }

    /** Perform narrow phase collision checks on all estimated dynamic vs static collisions.
     *  For collisions between non trigger colliders, use Epa to generate contacts. */
    void CollisionSystem::NarrowDetectVsStatic()
    {
        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<BoundingVolume>();
        const auto trigger = dynamicSoA->GetSpan<bool>();
        CollisionState state;

        for(auto& [dynamicIndex, staticPair] : m_broadEventsVsStatic)
        {
            INCREMENT_METRIC(metrics.staticNarrowChecks);
            const auto& dVolume = volumes[dynamicIndex];
            const auto& sVolume = staticPair->volume;
            const auto& dMatrix = matrices[dynamicIndex];
            const auto& sMatrix = staticPair->matrix;

            if(Gjk(dVolume, sVolume, dMatrix, sMatrix, &state))
            {
                INCREMENT_METRIC(metrics.staticCollisions);

                /** @todo See NarrowDetectVsDynamic */
                m_currentCollisions.emplace_back(handles[dynamicIndex], static_cast<EntityTraits::underlying_type>(staticPair->entity));

                if(!trigger[dynamicIndex] && !staticPair->isTrigger)
                {
                    Contact contact;
                    if(Epa(dVolume, sVolume, dMatrix, sMatrix, &state, &contact))
                    {
                        AddContact(handles[dynamicIndex], static_cast<EntityTraits::underlying_type>(staticPair->entity), contact);
                    }
                }
            }
        }
    }

    /** Add contact information from Epa to a persistent manifold. If no manifold exists between the objects, create one. */
    void CollisionSystem::AddContact(EntityTraits::underlying_type entityA, EntityTraits::underlying_type entityB, const Contact& contact)
    {
        auto pos = std::ranges::find_if(m_persistentManifolds, [entityA, entityB](const auto& manifold)
        {
            return (manifold.entityA == entityA && manifold.entityB == entityB) ||
                   (manifold.entityA == entityB && manifold.entityB == entityA);
        });

        if(pos == m_persistentManifolds.end())
        {
            Manifold newManifold{entityA, entityB, {contact}};
            m_persistentManifolds.push_back(newManifold);
            return;
        }

        pos->AddContact(contact);
    }

    /** Remove the contact manifold associated with two entities from the persistent collection. */
    void CollisionSystem::RemoveManifold(NarrowDetectEvent event)
    {
        auto pos = std::ranges::find_if(m_persistentManifolds, [event](const auto& manifold)
        {
            return (manifold.entityA == event.first && manifold.entityB == event.second) ||
                   (manifold.entityA == event.second && manifold.entityB == event.first);
        });

        if(pos != m_persistentManifolds.end())
        {
            *pos = m_persistentManifolds.back();
            m_persistentManifolds.pop_back();
        }
    }

    void CollisionSystem::FindExitAndStayEvents(registry_type* registry)
    {
        auto currBeg = m_currentCollisions.cbegin();
        auto currEnd = m_currentCollisions.cend();
        for(const auto& prev : m_previousCollisions)
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
            {
                NotifyCollisionExit(registry, prev);
                RemoveManifold(prev);
            }
            else
            {
                NotifyCollisionStay(registry, prev);
            }
        }
    }

    void CollisionSystem::FindEnterEvents(registry_type* registry) const
    {
        auto prevBeg = m_previousCollisions.cbegin();
        auto prevEnd = m_previousCollisions.cend();
        for(const auto& curr : m_currentCollisions)
        {
            if(prevEnd == std::find(prevBeg, prevEnd, curr))
                NotifyCollisionEnter(registry, curr);
        }
    }

    void CollisionSystem::NotifyCollisionEnter(registry_type* registry, const NarrowDetectEvent& data) const
    {
        auto h1 = static_cast<Entity>(data.first);
        auto h2 = static_cast<Entity>(data.second);
        if(registry->Contains<Entity>(h1))
            registry->Get<AutoComponentGroup>(h1)->SendOnCollisionEnter(h2);
        if(registry->Contains<Entity>(h2))
            registry->Get<AutoComponentGroup>(h2)->SendOnCollisionEnter(h1);
    }

    void CollisionSystem::NotifyCollisionExit(registry_type* registry, const NarrowDetectEvent& data) const
    {
        auto h1 = static_cast<Entity>(data.first);
        auto h2 = static_cast<Entity>(data.second);
        if(registry->Contains<Entity>(h1))
            registry->Get<AutoComponentGroup>(h1)->SendOnCollisionExit(h2);
        if(registry->Contains<Entity>(h2))
            registry->Get<AutoComponentGroup>(h2)->SendOnCollisionExit(h1);
    }

    void CollisionSystem::NotifyCollisionStay(registry_type* registry, const NarrowDetectEvent& data) const
    {
        auto h1 = static_cast<Entity>(data.first);
        auto h2 = static_cast<Entity>(data.second);
        if(registry->Contains<Entity>(h1))
            registry->Get<AutoComponentGroup>(h1)->SendOnCollisionStay(h2);
        if(registry->Contains<Entity>(h2))
            registry->Get<AutoComponentGroup>(h2)->SendOnCollisionStay(h1);
    }
} // namespace nc::phsyics