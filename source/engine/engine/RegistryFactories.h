#pragma once

#include <cstddef>

namespace nc
{
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

// Create a registry instance and register all engine components
auto BuildRegistry(size_t maxEntities) -> Registry;

// Create a module registry and register all engine modules
auto BuildModuleRegistry(Registry* registry,
                         window::WindowImpl* window,
                         const config::Config& config) -> ModuleRegistry;
} // namespace nc
