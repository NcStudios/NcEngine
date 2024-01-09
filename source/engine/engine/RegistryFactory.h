#pragma once

#include <cstddef>
#include <memory>

namespace nc
{
class Registry;

// Create a registry instance and register all engine components
auto BuildRegistry(size_t maxEntities) -> std::unique_ptr<Registry>;
} // namespace nc
