#pragma once

#include "physics/PhysicsSystem.h"
#include "ClickableSystem.h"
#include "PhysicsPipeline.h"
#include "collision/proxy/PerFrameProxyCache.h"
#include "collision/broad_phase/GlobalAllPair.h"
#include "collision/broad_phase/HashedGrid.h"

namespace nc::physics
{
    class PhysicsSystemImpl final : public PhysicsSystem
    {
        struct PipelineDescription
        {
            using multithreaded = std::true_type;
            using proxy_cache = PerFrameProxyCache;
            using proxy = proxy_cache::proxy_type;
            using broad_phase = HashedGrid<proxy_cache>;
            using concave_phase = BspTree;
        };

        public:
            PhysicsSystemImpl(Registry* registry);

            void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f) override;
            void RemoveJoint(Entity entityA, Entity entityB) override;
            void RemoveAllJoints(Entity entity) override;

            void RegisterClickable(IClickable* clickable) override;
            void UnregisterClickable(IClickable* clickable) noexcept override;
            auto RaycastToClickables(LayerMask mask = LayerMaskAll) -> IClickable* override;

            void DoPhysicsStep(tf::Executor& taskExecutor);
            void ClearState();

        private:
            PhysicsPipeline<PipelineDescription> m_pipeline;
            ClickableSystem m_clickableSystem;

            void BuildTaskGraph(Registry* registry);
    };
} // namespace nc::physics
