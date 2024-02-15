#pragma once

#include <memory>

namespace nc
{
struct SystemEvents;
class ModuleRegistry;
class Registry;

namespace config
{
struct Config;
} // namespace config

namespace window
{
class WindowImpl;
} // namespace window

// Create a module registry and register all engine modules
auto BuildModuleRegistry(Registry* registry,
                         SystemEvents& events,
                         window::WindowImpl* window,
                         const config::Config& config) -> std::unique_ptr<ModuleRegistry>;
} // namespace nc
