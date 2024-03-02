/**
 * @file NcEcs.h
 * @copyright Jaremie Romer and McCallister Romer 2024
 */
#pragma once

#include "ncengine/module/Module.h"
#include "ncengine/type/EngineId.h"

#include <memory>

namespace nc
{
struct SystemEvents;

namespace ecs
{
class ComponentRegistry;

/**
 * @brief Module managing ComponentRegistry operations.
 * 
 * Tasks
 *   Update FrameLogic Components
 *     Runs During: UpdatePhase::Logic
 *     Component Access: All
 *   Commit Pending Changes
 *     Runs During: UpdatePhase::Sync
 *     Component Access: All
 */
class NcEcs : public Module
{
    public:
        explicit NcEcs() noexcept
            : Module{NcEcsId} {}
};

/** @brief Build an NcEcs module instance. */
auto BuildEcsModule(ComponentRegistry& registry, SystemEvents& events) -> std::unique_ptr<NcEcs>;
} // namespace ecs
} // namespace nc
