#pragma once

#include "physics/PhysicsModule.h"
#include "ClickableSystem.h"
#include "PhysicsPipeline.h"
#include "collision/broad_phase/SingleAxisPrune.h"
#include "proxy/PerFrameProxyCache.h"

namespace nc::physics
{
    auto BuildPhysicsModule(bool enableModule, Registry* registry) -> std::unique_ptr<PhysicsModule>;

    class PhysicsModuleImpl final : public PhysicsModule
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
            PhysicsModuleImpl(Registry* registry);

            void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f) override;
            void RemoveJoint(Entity entityA, Entity entityB) override;
            void RemoveAllJoints(Entity entity) override;

            void RegisterClickable(IClickable* clickable) override;
            void UnregisterClickable(IClickable* clickable) noexcept override;
            auto RaycastToClickables(LayerMask mask = LayerMaskAll) -> IClickable* override;

            /** @todo not needed? */
            void DoPhysicsStep(tf::Executor& taskExecutor);

            void Clear() noexcept override;
            auto GetTasks() -> TaskGraph& override { return m_pipeline.GetTasks(); }

        private:
            PhysicsPipeline<PipelineDescription> m_pipeline;
            ClickableSystem m_clickableSystem;

            void BuildTaskGraph(Registry* registry);
    };
} // namespace nc::physics
