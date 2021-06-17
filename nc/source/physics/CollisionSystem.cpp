#include "CollisionSystem.h"
//#include "GjkEpa.h"
#include "Gjk.h"
#include "config/Config.h"
#include "debug/Profiler.h"
#include "directx/math/DirectXCollision.h"
#include "job/JobSystem.h"

#include <iostream>


namespace nc::physics
{


    SphereCollider EstimateBoundingVolume(const ecs::VolumeProperties& volumeProperties, DirectX::FXMMATRIX transform)
    {
        auto xExtent_v = DirectX::XMVector3Dot(transform.r[0], transform.r[0]);
        auto yExtent_v = DirectX::XMVector3Dot(transform.r[1], transform.r[1]);
        auto zExtent_v = DirectX::XMVector3Dot(transform.r[2], transform.r[2]);
        auto maxExtent_v = DirectX::XMVectorMax(xExtent_v, DirectX::XMVectorMax(yExtent_v, zExtent_v));
        float maxTransformExtent = sqrt(DirectX::XMVectorGetX(maxExtent_v));

        // can just translate?
        auto center_v = DirectX::XMLoadVector3(&volumeProperties.center);
        center_v = DirectX::XMVector3Transform(center_v, transform);
        
        //std::cout << "maxTransformsExtent:  " << maxTransformExtent << '\n'
        //          << "properties.maxExtent: " << volumeProperties.maxExtent << '\n';

        SphereCollider out{Vector3::Zero(), maxTransformExtent * volumeProperties.maxExtent};
        DirectX::XMStoreVector3(&out.center, center_v);
        return out;
    }

    bool operator ==(const NarrowDetectEvent& lhs, const NarrowDetectEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    CollisionSystem::CollisionSystem(ecs::ColliderSystem* colliderSystem, job::JobSystem* jobSystem)
        : m_colliderSystem{colliderSystem},
          m_jobSystem{jobSystem},
          m_dynamicEstimates{},
          m_broadEventsVsDynamic{},
          m_broadEventsVsStatic{},
          m_currentCollisions{},
          m_previousCollisions{}
    {
    }

    void CollisionSystem::DoCollisionStep()
    {
        #ifdef NC_EDITOR_ENABLED
        metrics.Reset();
        #endif

        FetchEstimates();

        auto broadDynamicJobResult = m_jobSystem->Schedule(BroadDetectVsDynamic, this);
        auto broadStaticJobResult = m_jobSystem->Schedule(BroadDetectVsStatic, this);
        broadDynamicJobResult.wait();
        broadStaticJobResult.wait();
        
        auto narrowDynamicJobResult = m_jobSystem->Schedule(NarrowDetectVsDynamic, this);
        auto narrowStaticJobResult = m_jobSystem->Schedule(NarrowDetectVsStatic, this);
        narrowDynamicJobResult.wait();
        broadStaticJobResult.wait();
        
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
    }

    void CollisionSystem::FetchEstimates()
    {
        const auto* soa = m_colliderSystem->GetDynamicSoA();
        auto [index, handles, matrices, properties] = soa->View<ecs::ColliderSystem::HandleTypeIndex,
                                                                ecs::ColliderSystem::MatrixIndex,
                                                                ecs::ColliderSystem::VolumePropertiesIndex>();
        auto* registry = ActiveRegistry();

        while(index.Valid())
        {
            matrices[index] = registry->Get<Transform>(static_cast<Entity>(handles[index]))->GetTransformationMatrix();
            m_dynamicEstimates.emplace_back(EstimateBoundingVolume(properties[index], matrices[index]), index);
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
                #ifdef NC_EDITOR_ENABLED
                ++metrics.dynamicBroadChecks;
                #endif

                if(BroadCollision(m_dynamicEstimates[i].estimate, m_dynamicEstimates[j].estimate))
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
                #ifdef NC_EDITOR_ENABLED
                ++metrics.staticBroadChecks;
                #endif

                // Because static volumes can exist in multiple tree nodes, identical intersections may be reported.
                const auto beg = m_broadEventsVsStatic.cbegin();
                const auto end = m_broadEventsVsStatic.cend();
                if(end == std::find_if(beg, end, [pair](const auto& event) { return event.second->entity == pair->entity; }))
                    m_broadEventsVsStatic.emplace_back(m_dynamicEstimates[i].index, pair);
            }
        }
    }

    void CollisionSystem::NarrowDetectVsDynamic()
    {
        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<Collider::BoundingVolume>();

        for(const auto& [i, j] : m_broadEventsVsDynamic)
        {
            #ifdef NC_EDITOR_ENABLED
            ++metrics.dynamicNarrowChecks;
            #endif

            if(GJK(volumes[i], volumes[j], matrices[i], matrices[j]))
                m_currentCollisions.emplace_back(handles[i], handles[j]);
        }
    }

    void CollisionSystem::NarrowDetectVsStatic()
    {
        auto* dynamicSoA = m_colliderSystem->GetDynamicSoA();
        const auto handles = dynamicSoA->GetSpan<EntityTraits::underlying_type>();
        const auto matrices = dynamicSoA->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = dynamicSoA->GetSpan<Collider::BoundingVolume>();
        for(auto& [dynamicIndex, staticPair] : m_broadEventsVsStatic)
        {
            #ifdef NC_EDITOR_ENABLED
            ++metrics.staticNarrowChecks;
            #endif

            if(GJK(volumes[dynamicIndex], staticPair->volume, matrices[dynamicIndex], staticPair->matrix))
                m_currentCollisions.emplace_back(handles[dynamicIndex], static_cast<EntityTraits::underlying_type>(staticPair->entity));
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
    }
} // namespace nc::phsyics