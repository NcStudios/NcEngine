#pragma once

#include <memory>

namespace nc
{
struct SystemEvents;
class ModuleRegistry;
class Registry;

namespace ecs
{
class ComponentRegistry;
} // namespace ecs

namespace config
{
struct Config;
} // namespace config

namespace task
{
class AsyncDispatcher;
} // namespace task

// Create a module registry and register all engine modules
auto BuildModuleRegistry(ecs::ComponentRegistry& registry,
                         const task::AsyncDispatcher& dispatcher,
                         SystemEvents& events,
                         const config::Config& config) -> std::unique_ptr<ModuleRegistry>;
} // namespace nc
