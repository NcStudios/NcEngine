#pragma once

#include "Manifold.h"
#include "ecs/ColliderTree.h"

#include <cstdint>
#include <vector>

namespace nc::physics
{
    /** Indices of two dynamic collider's positions in SoA.
     *  Produced by broad detection, consumed by narrow detection. */
    struct BroadDetectVsDynamicEvent
    {
        uint32_t first;
        uint32_t second;
    };

    /** Index of dynamic collider's SoA position and pointer to static
     *  collider tree entry. Produced by broad detection, consumed by
     *  narrow detection. */
    struct BroadDetectVsStaticEvent
    {
        uint32_t first;
        const ecs::StaticTreeEntry* second;
    };

    enum class CollisionEventType
    {
        Enter, Exit, Stay
    };

    /** Entitys of two colliding objects. Produced by narrow detection,
     *  consumed by compare/notify. */
    struct NarrowDetectEvent
    {
        EntityTraits::underlying_type first;
        EntityTraits::underlying_type second;
        CollisionEventType type;
    };

    inline bool operator ==(const NarrowDetectEvent& lhs, const NarrowDetectEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second) ||
               (lhs.first == rhs.second && lhs.second == rhs.first);
    }

    inline bool operator ==(const BroadDetectVsStaticEvent& lhs, const BroadDetectVsStaticEvent& rhs)
    {
        return (lhs.first == rhs.first && lhs.second == rhs.second);
    }



    class CollisionCache
    {
        public:
            void ClearState()
            {
                m_broadEventsVsDynamic.clear();
                m_broadEventsVsStatic.clear();
                m_currentCollisions.clear();
                m_previousCollisions.clear();
                m_manifolds.clear();
            }

            void EndStep()
            {
                m_previousCollisions = std::move(m_currentCollisions);
                m_currentCollisions.clear();
                m_broadEventsVsDynamic.clear();
                m_broadEventsVsStatic.clear();
            }

            void AddBroadVsDynamicEvent(uint32_t indexA, uint32_t indexB)
            {
                m_broadEventsVsDynamic.emplace_back(indexA, indexB);
            }

            bool AddBroadVsStaticEvent(uint32_t index, const ecs::StaticTreeEntry* treeEntry)
            {
                // Because static volumes can exist in multiple tree nodes, identical intersections may be reported.
                const auto beg = m_broadEventsVsStatic.cbegin();
                const auto end = m_broadEventsVsStatic.cend();

                auto newEvent = BroadDetectVsStaticEvent{index, treeEntry};
                if(end == std::find(beg, end, newEvent))
                {
                    m_broadEventsVsStatic.push_back(newEvent);
                    return true;
                }

                return false;
            }

            void AddNarrowDetectEvent(EntityTraits::underlying_type entityA, EntityTraits::underlying_type entityB)
            {
                m_currentCollisions.emplace_back(entityA, entityB);
            }

            void AddContact(EntityTraits::underlying_type entityA, EntityTraits::underlying_type entityB, const Contact& contact)
            {
                auto pos = std::ranges::find_if(m_manifolds, [entityA, entityB](const auto& manifold)
                {
                    return (manifold.entityA == entityA && manifold.entityB == entityB) ||
                           (manifold.entityA == entityB && manifold.entityB == entityA);
                });

                if(pos == m_manifolds.end())
                {
                    Manifold newManifold{entityA, entityB, {contact}};
                    m_manifolds.push_back(newManifold);
                    return;
                }

                pos->AddContact(contact);
            }

            void RemoveContact(NarrowDetectEvent event)
            {
                auto pos = std::ranges::find_if(m_manifolds, [event](const auto& manifold)
                {
                    return (manifold.entityA == event.first && manifold.entityB == event.second) ||
                           (manifold.entityA == event.second && manifold.entityB == event.first);
                });

                if(pos != m_manifolds.end())
                {
                    *pos = m_manifolds.back();
                    m_manifolds.pop_back();
                }
            }

            auto GetBroadVsDynamicEvents() const -> const std::vector<BroadDetectVsDynamicEvent>& { return m_broadEventsVsDynamic; }
            auto GetBroadVsStaticEvents() const -> const std::vector<BroadDetectVsStaticEvent>& { return m_broadEventsVsStatic; }
            auto GetCurrentCollisions() const -> const std::vector<NarrowDetectEvent>& { return m_currentCollisions; }
            auto GetPreviousCollisions() const -> const std::vector<NarrowDetectEvent>& { return m_previousCollisions; }
            
            auto GetManifolds() -> std::vector<Manifold>& { return m_manifolds; }

        private:
            std::vector<BroadDetectVsDynamicEvent> m_broadEventsVsDynamic;
            std::vector<BroadDetectVsStaticEvent> m_broadEventsVsStatic;
            std::vector<NarrowDetectEvent> m_currentCollisions;
            std::vector<NarrowDetectEvent> m_previousCollisions;
            std::vector<Manifold> m_manifolds;
    };
} // namespace nc::physics