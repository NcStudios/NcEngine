#pragma once

#include "physics/NcPhysics.h"
#include "ClickableSystem.h"
#include "PhysicsPipeline.h"
#include "collision/BspTree.h"
#include "collision/broad_phase/SingleAxisPrune.h"
#include "proxy/PerFrameProxyCache.h"
#include "task/TaskGraph.h"

namespace nc
{
struct SystemEvents;

namespace config
{
struct PhysicsSettings;
} // namespace config

namespace physics
{
/** @brief Factor for creating a physics module instance */
auto BuildPhysicsModule(const config::PhysicsSettings& settings, Registry* registry, SystemEvents& events) -> std::unique_ptr<NcPhysics>;

/** @brief Physics module implementation.
 * 
 * Handles the internal processing of Collider, ConcaveCollider, PhysicsBody, Joint, and IClickable types. */
class NcPhysicsImpl final : public NcPhysics
{
    struct PipelineDescription
    {
        using multithreaded = std::true_type;
        using proxy_cache = PerFrameProxyCache<Registry>;
        using proxy = proxy_cache::proxy_type;
        using broad_phase = SingleAxisPrune<proxy_cache>;
        using concave_phase = BspTree;
    };

    public:
        NcPhysicsImpl(const config::PhysicsSettings& settings, Registry* registry, SystemEvents& events);

        void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f) override;
        void RemoveJoint(Entity entityA, Entity entityB) override;
        void RemoveAllJoints(Entity entity) override;
        void RegisterClickable(IClickable* clickable) override;
        void UnregisterClickable(IClickable* clickable) noexcept override;
        auto RaycastToClickables(LayerMask mask = LayerMaskAll) -> IClickable* override;
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&) override;
        void Clear() noexcept override;

    private:
        PhysicsPipeline<PipelineDescription> m_pipeline;
        ClickableSystem m_clickableSystem;
        float m_accumulatedTime;
        unsigned m_currentIterations;
};
} // namespace physics
} // namespace nc
