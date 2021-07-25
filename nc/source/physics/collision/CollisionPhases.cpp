#include "CollisionPhases.h"
#include "IntersectionQueries.h"

#include <iostream>

namespace
{
    using namespace nc;
    using namespace nc::physics;

    void AddContact(Entity entityA, Entity entityB, const Contact& contact, std::vector<Manifold>& manifolds)
    {
        auto pos = std::ranges::find_if(manifolds, [entityA, entityB](const auto& manifold)
        {
            return (manifold.entityA == entityA && manifold.entityB == entityB) ||
                   (manifold.entityA == entityB && manifold.entityB == entityA);
        });

        if(pos == manifolds.end())
        {
            Manifold newManifold{entityA, entityB, {contact}};
            manifolds.push_back(newManifold);
            return;
        }

        pos->AddContact(contact);
    }
}

namespace nc::physics
{
    void FetchEstimates(registry_type* registry,
                        ecs::ColliderSystem::DynamicColliderSoA* soa,
                        std::vector<DynamicEstimate>& estimates)
    {
        auto [index, handles, matrices, volumes, trigger] = soa->View<ecs::ColliderSystem::HandleIndex,
                                                                      ecs::ColliderSystem::MatrixIndex,
                                                                      ecs::ColliderSystem::BoundingVolumeIndex,
                                                                      ecs::ColliderSystem::TriggerIndex>();

        while(index.Valid())
        {
            auto interactionType = trigger[index] ? CollisionInteractionType::Trigger : CollisionInteractionType::Physics;
            matrices[index] = registry->Get<Transform>(handles[index])->GetTransformationMatrix();
            estimates.emplace_back(EstimateBoundingVolume(volumes[index], matrices[index]), index, interactionType);
            ++index;
        }
    }

    void UpdateManifolds(registry_type* registry, std::vector<Manifold>& manifolds)
    {
        for(auto& manifold : manifolds)
        {
            manifold.UpdateWorldPoints(registry);
        }
    }

    void FindBroadPairs(const std::vector<DynamicEstimate>& estimates,
                        std::vector<BroadDetectVsDynamicEvent>& broadPhysicsEvents,
                        std::vector<BroadDetectVsDynamicEvent>& broadTriggerEvents)
    {
        const auto count = estimates.size();

        for(size_t i = 0u; i < count; ++i)
        {
            for(size_t j = i + 1; j < count; ++j)
            {
                const auto& first = estimates[i];
                const auto& second = estimates[j];

                if(first.interactionType == CollisionInteractionType::Trigger && second.interactionType == CollisionInteractionType::Trigger)
                    continue;

                if(Intersect(first.estimate, second.estimate))
                {
                    if(first.interactionType == CollisionInteractionType::Trigger || second.interactionType == CollisionInteractionType::Trigger)
                        broadTriggerEvents.emplace_back(first.index, second.index);
                    else
                        broadPhysicsEvents.emplace_back(first.index, second.index);
                }
            }
        }
    }

    void FindBroadStaticPairs(const std::vector<DynamicEstimate>& estimates,
                              const ecs::ColliderTree* staticTree,
                              std::vector<BroadDetectVsStaticEvent>& broadStaticPhysicsEvents,
                              std::vector<BroadDetectVsStaticEvent>& broadStaticTriggerEvents)
    {
        const auto count = estimates.size();

        for(size_t i = 0u; i < count; ++i)
        {
            for(const auto* staticEntry : staticTree->BroadCheck(estimates[i].estimate))
            {
                bool estimateIsTrigger = (estimates[i].interactionType == CollisionInteractionType::Trigger) ? true : false;

                if(estimateIsTrigger && staticEntry->isTrigger)
                    continue;

                auto& container = (estimateIsTrigger || staticEntry->isTrigger) ? broadStaticTriggerEvents : broadStaticPhysicsEvents;
                const auto beg = container.cbegin();
                const auto end = container.cend();
                auto newEvent = BroadDetectVsStaticEvent{estimates[i].index, staticEntry};
                
                if(end == std::find(beg, end, newEvent))
                    container.push_back(newEvent);
            }
        }
    }

    void FindNarrowPhysicsPairs(const std::vector<BroadDetectVsDynamicEvent>& broadPhysicsEvents,
                                const std::vector<BroadDetectVsStaticEvent>& broadStaticPhysicsEvents,
                                const ecs::ColliderSystem::DynamicColliderSoA* soa,
                                std::vector<NarrowDetectEvent>& narrowPhysicsEvents,
                                std::vector<Manifold>& manifolds)
    {
        const auto handles = soa->GetSpan<Entity>();
        const auto matrices = soa->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = soa->GetSpan<BoundingVolume>();
        CollisionState state;

        for(const auto& [i, j] : broadPhysicsEvents)
        {
            if(Gjk(volumes[i], volumes[j], matrices[i], matrices[j], &state))
            {
                if(Epa(volumes[i], volumes[j], matrices[i], matrices[j], &state))
                {
                    narrowPhysicsEvents.emplace_back(handles[i], handles[j]);
                    AddContact(handles[i], handles[j], state.contact, manifolds);
                }
            }
        }

        for(const auto& [dynamicIndex, staticEntry] : broadStaticPhysicsEvents)
        {
            if(Gjk(volumes[dynamicIndex], staticEntry->volume, matrices[dynamicIndex], staticEntry->matrix, &state))
            {
                if(Epa(volumes[dynamicIndex], staticEntry->volume, matrices[dynamicIndex], staticEntry->matrix, &state))
                {
                    narrowPhysicsEvents.emplace_back(handles[dynamicIndex], staticEntry->entity);
                    AddContact(handles[dynamicIndex], staticEntry->entity, state.contact, manifolds);
                }
            }
        }
    }

    void FindNarrowTriggerPairs(const std::vector<BroadDetectVsDynamicEvent>& broadTriggerEvents,
                                const std::vector<BroadDetectVsStaticEvent>& broadStaticTriggerEvents,
                                const ecs::ColliderSystem::DynamicColliderSoA* soa,
                                std::vector<NarrowDetectEvent>& narrowTriggerEvents)
    {
        const auto handles = soa->GetSpan<Entity>();
        const auto matrices = soa->GetSpan<DirectX::XMMATRIX>();
        const auto volumes = soa->GetSpan<BoundingVolume>();
        CollisionState state;

        for(const auto& [i, j] : broadTriggerEvents)
        {
            if(Gjk(volumes[i], volumes[j], matrices[i], matrices[j], &state))
            {
                narrowTriggerEvents.emplace_back(handles[i], handles[j]);
            }
        }

        for(const auto& [dynamicIndex, staticEntry] : broadStaticTriggerEvents)
        {
            if(Gjk(volumes[dynamicIndex], staticEntry->volume, matrices[dynamicIndex], staticEntry->matrix, &state))
            {
                narrowTriggerEvents.emplace_back(handles[dynamicIndex], staticEntry->entity);
            }
        }
    }
}