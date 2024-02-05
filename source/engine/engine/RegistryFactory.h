#pragma once

#include <cstddef>
#include <memory>

namespace nc
{
namespace ecs
{
class ComponentRegistry;
} // namespace ecs

// Create a registry instance and register all engine components
auto BuildRegistry(size_t maxEntities) -> std::unique_ptr<ecs::ComponentRegistry>;
} // namespace nc
