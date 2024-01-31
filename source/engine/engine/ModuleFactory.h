#pragma once

#include <functional>
#include <memory>

namespace nc
{
class ModuleRegistry;
class Registry;
class Scene;

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
                         window::WindowImpl* window,
                         const config::Config& config,
                         std::function<void(std::unique_ptr<Scene>)> changeScene) -> ModuleRegistry;

// Create a registry instance and register all engine components
auto BuildRegistry(size_t maxEntities) -> std::unique_ptr<Registry>;
} // namespace nc
