#pragma once

#include "utility/Signal.h"

namespace nc::graphics
{
struct EnvironmentData;
struct ObjectData;
struct PointLightData;

/** @brief Data transfer channels for shader resources. Signals are emitted from frontend graphics
 *         systems to be picked up by API-specific backend handlers. */
struct ShaderResourceBus
{
    Signal<const EnvironmentData&> environmentChannel;
    Signal<const std::vector<ObjectData>&> objectChannel;
    Signal<const std::vector<PointLightData>&> pointLightChannel;
};
} // namespace nc::graphics