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
          m_manifolds{}
    {
    }

    void CollisionSystem::DoCollisionStep()
    {
        #ifdef NC_EDITOR_ENABLED
        metrics.Reset();
        #endif

        /** @todo re-visit JobSystem here */ 
        FetchEstimates();
        BroadDetectVsStatic();
        BroadDetectVsDynamic();
        NarrowDetectVsStatic();
        NarrowDetectVsDynamic();
        ResolveCollisions();
        FindExitAndStayEvents();
        FindEnterEvents();

        Cleanup();
    }

    void CollisionSystem::ClearState()
    {
        m_broadEventsVsDynamic.resize(0u);
        m_broadEventsVsStatic.resize(0u);
        m_currentCollisions.resize(0u);
        m_previousCollisions.resize(0u);
        m_manifolds.resize(0u);
    }

    void CollisionSystem::FetchEstimates()
    {
        const auto* soa = m_colliderSystem->GetDynamicSoA();
        auto [index, handles, matrices, volumes] = soa->View<ecs::ColliderSystem::HandleTypeIndex,
                                                    ecs::ColliderSystem::MatrixIndex,
                                                    ecs::ColliderSystem::BoundingVolumeIndex>();
        auto* registry = ActiveRegistry();

        while(index.Valid())
        {
            matrices[index] = registry->Get<Transform>(static_cast<Entity>(handles[index]))->GetTransformationMatrix();
            m_dynamicEstimates.emplace_back(EstimateBoundingVolume(volumes[index], matrices[index]), index);
            ++index;
        }
    }

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

    void CollisionSystem::NarrowDetectVsDynamic()
    {
        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<BoundingVolume>();
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
                 *  of m_currentCollisions, but that won't work for trigger colliders
                 *  (once they are added). Leaving m_currentCollisions unchanged for now. */
                m_currentCollisions.emplace_back(handles[i], handles[j]);
                auto contactData = Epa(iVolume, jVolume, iMatrix, jMatrix, &state);
                m_manifolds.emplace_back(handles[i], handles[j], contactData);
            }
        }
    }

    void CollisionSystem::NarrowDetectVsStatic()
    {
        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<BoundingVolume>();
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
                auto contactData = Epa(dVolume, sVolume, dMatrix, sMatrix, &state);
                m_manifolds.emplace_back(handles[dynamicIndex], static_cast<EntityTraits::underlying_type>(staticPair->entity), contactData);
            }
        }
    }
    
    void CollisionSystem::ResolveCollisions()
    {
        for(auto& manifold : m_manifolds)
        {
            auto a = Entity{manifold.entityA};
            auto b = Entity{manifold.entityB};
            auto mtv = manifold.contact.normal * manifold.contact.distance;
            auto* registry = ActiveRegistry();
            auto* transformA = registry->Get<Transform>(a);
            auto* transformB = registry->Get<Transform>(b);

            if(!EntityUtils::IsStatic(a))
                transformA->Translate(-mtv);
            if(!EntityUtils::IsStatic(b))
                transformB->Translate(mtv);
        }
    }

    void CollisionSystem::FindExitAndStayEvents() const
    {
        auto currBeg = m_currentCollisions.cbegin();
        auto currEnd = m_currentCollisions.cend();
        for(const auto& prev : m_previousCollisions)
        {
            if(currEnd == std::find(currBeg, currEnd, prev))
                NotifyCollisionEvent(prev, CollisionEventType::Exit);
            else
                NotifyCollisionEvent(prev, CollisionEventType::Stay);
        }
    }

    void CollisionSystem::FindEnterEvents() const
    {
        auto prevBeg = m_previousCollisions.cbegin();
        auto prevEnd = m_previousCollisions.cend();
        for(const auto& curr : m_currentCollisions)
        {
            if(prevEnd == std::find(prevBeg, prevEnd, curr))
                NotifyCollisionEvent(curr, CollisionEventType::Enter);
        }
    }

    void CollisionSystem::NotifyCollisionEvent(const NarrowDetectEvent& data, CollisionEventType type) const
    {
        auto h1 = static_cast<Entity>(data.first);
        auto h2 = static_cast<Entity>(data.second);
        auto* reg = ActiveRegistry();

        switch(type)
        {
            case CollisionEventType::Enter:
            {
                if(reg->Contains<Entity>(h1))
                    reg->Get<AutoComponentGroup>(h1)->SendOnCollisionEnter(h2);
                if(reg->Contains<Entity>(h2))
                    reg->Get<AutoComponentGroup>(h2)->SendOnCollisionEnter(h1);
                break;
            }
            case CollisionEventType::Stay:
            {
                if(reg->Contains<Entity>(h1))
                    reg->Get<AutoComponentGroup>(h1)->SendOnCollisionStay(h2);
                if(reg->Contains<Entity>(h2))
                    reg->Get<AutoComponentGroup>(h2)->SendOnCollisionStay(h1);
                break;
            }
            case CollisionEventType::Exit:
            {
                if(reg->Contains<Entity>(h1))
                    reg->Get<AutoComponentGroup>(h1)->SendOnCollisionExit(h2);
                if(reg->Contains<Entity>(h2))
                    reg->Get<AutoComponentGroup>(h2)->SendOnCollisionExit(h1);
                break;
            }
            default:
                throw std::runtime_error("NotifyCollisionEvent - Unknown CollisionEventType");
        }
    }

    void CollisionSystem::Cleanup()
    {
        m_previousCollisions = std::move(m_currentCollisions);
        m_currentCollisions.clear();
        m_dynamicEstimates.clear();
        m_broadEventsVsDynamic.clear();
        m_broadEventsVsStatic.clear();
        m_manifolds.clear();
    }
} // namespace nc::phsyics