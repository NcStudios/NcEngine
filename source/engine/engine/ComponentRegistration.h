#pragma once

namespace nc
{
class Registry;

// Register all engine-side Component types with the Registry.
void RegisterEngineComponents(Registry& registry);
} // namespace nc
