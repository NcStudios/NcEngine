#pragma once

#include "module/Module.h"

#include <memory>

namespace nc { class Registry; }

namespace nc::ecs
{
/** Module for updating FrameLogic components */
class LogicModule : public Module
{
    public:
        LogicModule(Registry* registry) noexcept;

        auto BuildWorkload() -> std::vector<task::Job> override;
        void Run();

    private:
        Registry* m_registry;
};

auto BuildLogicModule(Registry* registry) -> std::unique_ptr<LogicModule>;
} // namespace nc::ecs
