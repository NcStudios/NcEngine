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
    CollisionSystem::CollisionSystem(ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
        : m_colliderSystem{colliderSystem},
          m_jobSystem{jobSystem},
          m_dynamicEstimates{}
    {
    }

    auto CollisionSystem::DoCollisionStep(registry_type* registry) -> const std::vector<Manifold>&
    {
        #ifdef NC_EDITOR_ENABLED
        metrics.Reset();
        #endif

        NC_PROFILE_BEGIN(debug::profiler::Filter::Collision);

        for(auto& manifold : m_cache.GetManifolds())
        {
            manifold.UpdateWorldPoints(registry);
        }

        /** @todo re-visit JobSystem here */ 
        FetchEstimates(registry);
        BroadDetectVsStatic();
        BroadDetectVsDynamic();
        NarrowDetectVsStatic();
        NarrowDetectVsDynamic();

        NC_PROFILE_END();

        return m_cache.GetManifolds();
    }

    void CollisionSystem::NotifyCollisionEvents(registry_type* registry)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Collision);
        FindExitAndStayEvents(registry);
        FindEnterEvents(registry);
        NC_PROFILE_END();
    }

    /** End of frame/step cleanup. */
    void CollisionSystem::Cleanup()
    {
        m_cache.EndStep();
        m_dynamicEstimates.clear();
    }

    /** Reset all state. */
    void CollisionSystem::ClearState()
    {
        m_cache.ClearState();
    }

    /** Update locally stored transformation matrices and estimated bounding volumes
     *  for each dynamic collider. */
    void CollisionSystem::FetchEstimates(registry_type* registry)
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Collision);

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

        NC_PROFILE_END();
    }

    /** Check for intersection between all dynamic volume estimates. */
    void CollisionSystem::BroadDetectVsDynamic()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Collision);

        const auto dynamicSize = m_dynamicEstimates.size();
        for(size_t i = 0u; i < dynamicSize; ++i)
        {
            for(size_t j = i + 1; j < dynamicSize; ++j)
            {
                INCREMENT_METRIC(metrics.dynamicBroadChecks);

                if(Intersect(m_dynamicEstimates[i].estimate, m_dynamicEstimates[j].estimate))
                    m_cache.AddBroadVsDynamicEvent(m_dynamicEstimates[i].index, m_dynamicEstimates[j].index);
            }
        }

        NC_PROFILE_END();
    }

    /** Check for intersection between dynamic volume estimates and static estimates. */
    void CollisionSystem::BroadDetectVsStatic()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Collision);

        const auto dynamicSize = m_dynamicEstimates.size();
        auto* staticTree = m_colliderSystem->GetStaticTree();
        for(size_t i = 0u; i < dynamicSize; ++i)
        {
            for(const auto* pair : staticTree->BroadCheck(m_dynamicEstimates[i].estimate))
            {
                if(m_cache.AddBroadVsStaticEvent(m_dynamicEstimates[i].index, pair))
                {
                    INCREMENT_METRIC(metrics.staticBroadChecks);
                }
            }
        }

        NC_PROFILE_END();
    }

    /** Perform narrow phase collision checks on all estimated dynamic vs. dynamic collisions.
     *  For collisions between non trigger colliders, use Epa to generate contacts. */
    void CollisionSystem::NarrowDetectVsDynamic()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Collision);
        
        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<BoundingVolume>();
        const auto trigger = dynamicSoA->GetSpan<bool>();
        CollisionState state;

        for(const auto& [i, j] : m_cache.GetBroadVsDynamicEvents())
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
                m_cache.AddNarrowDetectEvent(handles[i], handles[j]);

                if(!trigger[i] && !trigger[j])
                {
                    Contact contact;
                    if(Epa(iVolume, jVolume, iMatrix, jMatrix, &state, &contact))
                    {
                        m_cache.AddContact(handles[i], handles[j], contact);
                    }
                }
            }
        }

        NC_PROFILE_END();
    }

    /** Perform narrow phase collision checks on all estimated dynamic vs static collisions.
     *  For collisions between non trigger colliders, use Epa to generate contacts. */
    void CollisionSystem::NarrowDetectVsStatic()
    {
        NC_PROFILE_BEGIN(debug::profiler::Filter::Collision);

        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<BoundingVolume>();
        const auto trigger = dynamicSoA->GetSpan<bool>();
        CollisionState state;

        for(const auto& [dynamicIndex, staticPair] : m_cache.GetBroadVsStaticEvents())
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
                m_cache.AddNarrowDetectEvent(handles[dynamicIndex], static_cast<EntityTraits::underlying_type>(staticPair->entity));

                if(!trigger[dynamicIndex] && !staticPair->isTrigger)
                {
                    Contact contact;
                    if(Epa(dVolume, sVolume, dMatrix, sMatrix, &state, &contact))
                    {
                        m_cache.AddContact(handles[dynamicIndex], static_cast<EntityTraits::underlying_type>(staticPair->entity), contact);
                    }
                }
            }
        }

        NC_PROFILE_END();
    }

    void CollisionSystem::FindExitAndStayEvents(registry_type* registry)
    {
        const auto& currentEvents = m_cache.GetCurrentCollisions();
        auto currBeg = currentEvents.cbegin();
        auto currEnd = currentEvents.cend();

        for(const auto& prev : m_cache.GetPreviousCollisions())
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
            {
                NotifyCollisionExit(registry, prev);
                m_cache.RemoveContact(prev);
            }
            else
            {
                NotifyCollisionStay(registry, prev);
            }
        }
    }

    void CollisionSystem::FindEnterEvents(registry_type* registry) const
    {
        const auto& previousEvents = m_cache.GetPreviousCollisions();
        auto prevBeg = previousEvents.cbegin();
        auto prevEnd = previousEvents.cend();

        for(const auto& curr : m_cache.GetCurrentCollisions())
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