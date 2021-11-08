#pragma once

#include "physics/PhysicsSystem.h"
#include "ClickableSystem.h"
#include "collision/CollisionCache.h"
#include "collision/BspTree.h"
#include "dynamics/Joint.h"
#include "graphics/DebugRenderer.h"
#include "task/Task.h"

#include "PhysicsWorld.h"
//#include "PhysicsPipelineTypes.h"
//#include "collision/proxy/FrameDeltaProxyCache.h"
#include "PhysicsPipeline.h"

namespace nc::physics
{
    class PhysicsSystemImpl final : public PhysicsSystem
    {
        //using pipeline = PhysicsPipelineDescription<PerFrameProxyCache, HashedGrid, NarrowPhase>;
        using pipeline = PhysicsPipelineDescription<PerFrameProxyCache, GlobalAllPair, NarrowPhase>;

        public:
            PhysicsSystemImpl(Registry* registry, graphics::Graphics* graphics);

            void AddJoint(Entity entityA, Entity entityB, const Vector3& anchorA, const Vector3& anchorB, float bias = 0.2f, float softness = 0.0f) override;
            void RemoveJoint(Entity entityA, Entity entityB) override;
            void RemoveAllJoints(Entity entity) override;

            void RegisterClickable(IClickable* clickable) override;
            void UnregisterClickable(IClickable* clickable) noexcept override;
            auto RaycastToClickables(LayerMask mask = LayerMaskAll) -> IClickable* override;

            void DoPhysicsStep(tf::Executor& taskExecutor);
            void ClearState();

            #ifdef NC_DEBUG_RENDERING
            void DebugRender();
            #endif

        private:
            CollisionCache m_cache;
            std::vector<Joint> m_joints;
            BspTree m_bspTree;
            ClickableSystem m_clickableSystem;
            TaskGraph m_tasks;
            //PhysicsWorld m_world;

            //PhysicsPipelineBuilder<pipeline> m_pipeline;

            pipeline::broad_phase::result_type m_broadPairs;
            pipeline::broad_phase::result_type m_broadPairs2;
            pipeline::proxy_cache m_proxyCache;
            pipeline::broad_phase m_broadPhase;
            pipeline::narrow_phase m_narrowPhase;
            #ifdef NC_DEBUG_RENDERING
            graphics::DebugRenderer m_debugRenderer;
            #endif

            void BuildTaskGraph(Registry* registry);
    };
} // namespace nc::physics
