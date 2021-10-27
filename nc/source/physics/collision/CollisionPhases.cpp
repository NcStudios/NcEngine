#include "CollisionPhases.h"
#include "IntersectionQueries.h"
#include "debug/NcError.h"
#include "ecs/Registry.h"
#include "physics/PhysicsConstants.h"

namespace
{
    using namespace nc;
    using namespace nc::physics;

    auto GetColliderInteractionType(bool isTrigger, const PhysicsBody* body) -> ColliderInteractionType
    {
        // note: could probably store this in collider... have physics body send message in c'tor
        // to update as it already gets the collider pointer

        if(body)
        {
            if(body->IsKinematic())
            {
                if(isTrigger)
                {
                    return ColliderInteractionType::KinematicPhysicsTrigger;
                }

                return ColliderInteractionType::KinematicPhysics;
            }

            // need to add kinematic body
            if(isTrigger)
            {
                return ColliderInteractionType::PhysicsTrigger;
            }

            return ColliderInteractionType::Physics;
        }

        if(isTrigger)
        {
            return ColliderInteractionType::ColliderTrigger;
        }

        return ColliderInteractionType::Collider;

    }

    CollisionEventType GetInteractionType(ColliderInteractionType typeA, ColliderInteractionType typeB)
    {
        if(typeA == ColliderInteractionType::Collider)
        {
            if(typeB == ColliderInteractionType::Physics)
                return CollisionEventType::SecondBodyPhysics;
            
            if(typeB == ColliderInteractionType::PhysicsTrigger || typeB == ColliderInteractionType::KinematicPhysicsTrigger)
                return CollisionEventType::Trigger;
            
            return CollisionEventType::None;
        }

        if(typeA == ColliderInteractionType::Physics)
        {
            if(typeB == ColliderInteractionType::Physics)
                return CollisionEventType::TwoBodyPhysics;
            
            if(typeB == ColliderInteractionType::Collider || typeB == ColliderInteractionType::KinematicPhysics)
                return CollisionEventType::FirstBodyPhysics;

            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::KinematicPhysics)
        {
            if(typeB == ColliderInteractionType::Physics)
                return CollisionEventType::SecondBodyPhysics;
            
            if(typeB == ColliderInteractionType::Collider || typeB == ColliderInteractionType::KinematicPhysics)
                return CollisionEventType::None;
            
            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::ColliderTrigger)
        {
            if(typeB == ColliderInteractionType::Collider || typeB == ColliderInteractionType::ColliderTrigger)
                return CollisionEventType::None;
            
            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::PhysicsTrigger)
        {
            return CollisionEventType::Trigger;
        }

        if(typeA == ColliderInteractionType::KinematicPhysicsTrigger)
        {
            return CollisionEventType::Trigger;
        }

        throw NcError("Unknown ColliderInteractionType");
    }
}

namespace nc::physics
{
    void FetchEstimates(const Registry* registry, CollisionStepInitData* out)
    {
        auto colliders = registry->ViewAll<Collider>();
        auto colliderCount = colliders.size();
        out->estimates.clear();
        out->estimates.reserve(colliderCount);
        out->matrices.clear();
        out->matrices.reserve(colliderCount);

        for(uint32_t i = 0u; i < colliderCount; ++i)
        {
            const auto& collider = colliders[i];
            auto entity = collider.GetParentEntity();
            const auto* body = registry->Get<PhysicsBody>(entity);
            auto interactionType = GetColliderInteractionType(collider.IsTrigger(), body);
            out->matrices.push_back(registry->Get<Transform>(entity)->GetTransformationMatrix());
            out->estimates.emplace_back(collider.EstimateBoundingVolume(out->matrices.back()), i, interactionType, collider.IsAwake());
        }
    }

    void UpdateManifolds(const Registry* registry, std::vector<Manifold>& manifolds)
    {
        for(auto cur = manifolds.rbegin(); cur != manifolds.rend(); ++cur)
        {
            cur->UpdateWorldPoints(registry);
            
            if(cur->contacts.empty())
            {
                *cur = std::move(manifolds.back());
                manifolds.pop_back();
            }
        }
    }

    void MergeNewContacts(const NarrowPhysicsResult& newEvents, std::vector<Manifold>& manifolds)
    {
        IF_THROW(newEvents.events.size() != newEvents.events.size(), "MergeNewContacts - NarrowPhaseResults are not the same size");

        const auto eventCount = newEvents.contacts.size();
        for(size_t i = 0u; i < eventCount; ++i)
        {
            const auto& [entityA, entityB, eventType] = newEvents.events[i];
            auto pos = std::ranges::find_if(manifolds, [entityA, entityB](const auto& manifold)
            {
                return (manifold.entityA == entityA && manifold.entityB == entityB) ||
                       (manifold.entityA == entityB && manifold.entityB == entityA);
            });

            if(pos == manifolds.end())
            {
                Manifold newManifold{entityA, entityB, eventType, {newEvents.contacts[i]}};
                manifolds.push_back(newManifold);
                continue;
            }

            pos->AddContact(newEvents.contacts[i]);
        }
    }

    void FindBroadPairs(std::span<const ColliderEstimate> estimates, size_t prevPhysicsCount, size_t prevTriggerCount, BroadResult* out)
    {
        out->physics.clear();
        out->physics.reserve(prevPhysicsCount);
        out->trigger.clear();
        out->trigger.reserve(prevTriggerCount);

        const auto count = estimates.size();

        for(size_t i = 0u; i < count; ++i)
        {
            const auto& first = estimates[i];

            for(size_t j = i + 1; j < count; ++j)
            {
                const auto& second = estimates[j];

                if constexpr(physics::EnableSleeping)
                {
                    if(!first.isAwake && !second.isAwake)
                        continue;
                }

                auto interactionType = GetInteractionType(first.interactionType, second.interactionType);
                if(interactionType == CollisionEventType::None)
                    continue;

                if(Intersect(first.estimate, second.estimate))
                {
                    if(interactionType == CollisionEventType::Trigger)
                        out->trigger.emplace_back(first.index, second.index, interactionType);
                    else
                        out->physics.emplace_back(first.index, second.index, interactionType);
                }
            }
        }
    }

    void FindNarrowPhysicsPairs(Registry* registry, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadPhysicsEvents, NarrowPhysicsResult* out)
    {
        const auto broadEventCount = broadPhysicsEvents.size();
        out->contacts.clear();
        out->contacts.reserve(broadEventCount);
        out->events.clear();
        out->events.reserve(broadEventCount);
        CollisionState state;
        auto colliders = registry->ViewAll<Collider>();

        for(auto& [i, j, eventType] : broadPhysicsEvents)
        {
            auto& collider1 = colliders[i];
            auto& collider2 = colliders[j];
            const auto& v1 = collider1.GetVolume();
            const auto& v2 = collider2.GetVolume();
            const auto& m1 = matrices[i];
            const auto& m2 = matrices[j];

            if(Collide(v1, v2, m1, m2, &state))
            {
                auto e1 = collider1.GetParentEntity();
                auto e2 = collider2.GetParentEntity();

                if constexpr(EnableSleeping)
                {
                    if(!collider1.IsAwake())
                    {
                        collider1.Wake();
                        registry->Get<PhysicsBody>(e1)->Wake();
                    }

                    if(!collider2.IsAwake())
                    {
                        collider2.Wake();
                        registry->Get<PhysicsBody>(e2)->Wake();
                    }
                }

                out->events.emplace_back(e1, e2, eventType);
                out->contacts.push_back(state.contact);
            }
        }
    }

    void FindNarrowTriggerPairs(const Registry* registry, std::span<const DirectX::XMMATRIX> matrices, std::span<const BroadEvent> broadTriggerEvents, std::vector<NarrowEvent>* out)
    {
        out->clear();
        out->reserve(broadTriggerEvents.size());
        auto colliders = registry->ViewAll<Collider>();
        CollisionState state;

        for(const auto& [i, j, unused] : broadTriggerEvents)
        {
            const auto& v1 = colliders[i].GetVolume();
            const auto& v2 = colliders[j].GetVolume();
            const auto& m1 = matrices[i];
            const auto& m2 = matrices[j];

            if(Intersect(v1, v2, m1, m2))
            {
                out->emplace_back(colliders[i].GetParentEntity(), colliders[j].GetParentEntity());
            }
        }
    }
}