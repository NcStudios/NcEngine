#pragma once

#include "module/Module.h"

#include <memory>

namespace nc { class Registry; }

namespace nc::ecs
{
/** Module for updating FrameLogic components and synchronizing the Registry. */
class EcsModule : public Module
{
    public:
        EcsModule(Registry* registry) noexcept;

        void OnBuildTaskGraph(task::TaskGraph&) override;
        void RunFrameLogic();

    private:
        Registry* m_registry;

        void UpdateWorldSpaceMatrices();
};

auto BuildEcsModule(Registry* registry) -> std::unique_ptr<EcsModule>;
} // namespace nc::ecs
