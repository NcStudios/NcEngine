#pragma once

#include "ncengine/module/Module.h"
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
class EcsModule : public Module
{
    public:
        EcsModule(Registry* registry, SystemEvents& events) noexcept;

        void OnBuildTaskGraph(task::TaskGraph&) override;
        void RunFrameLogic();

    private:
        Registry* m_registry;
        Connection<> m_rebuildStaticConnection;

        void UpdateWorldSpaceMatrices();
        void UpdateStaticWorldSpaceMatrices();
};

auto BuildEcsModule(Registry* registry, SystemEvents& events) -> std::unique_ptr<EcsModule>;
} // namespace nc::ecs
