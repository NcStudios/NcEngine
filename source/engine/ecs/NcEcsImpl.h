#pragma once

#include "ncengine/ecs/NcEcs.h"
#include "ncengine/utility/Signal.h"

#include <memory>

namespace nc
{
struct SystemEvents;
class Registry;
}

namespace nc::ecs
{
/** Module for updating FrameLogic components and synchronizing the Registry. */
class EcsModule : public NcEcs
{
    public:
        EcsModule(ComponentRegistry& registry, SystemEvents& events) noexcept;

        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks&) override;
        void RunFrameLogic();

    private:
        ComponentRegistry* m_registry;
        Connection<> m_rebuildStaticConnection;

        void UpdateWorldSpaceMatrices();
        void UpdateStaticWorldSpaceMatrices();
};

auto BuildEcsModule(ComponentRegistry* registry, SystemEvents& events) -> std::unique_ptr<NcEcs>;
} // namespace nc::ecs
